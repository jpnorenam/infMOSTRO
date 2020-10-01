import os
import numpy as np
import pandas as pd
import matplotlib
import matplotlib.pyplot as plt

def routes_flow(xsection_name, source, workspace_dir, fv, datacarriles_list, edges_dict, routes_dict):
    print("[pyinfmostro {}] reconciling measurement errors and balancing routes demand.".format(xsection_name))
    edges = {}
    edges_entries = []; edges_sinks = []
    entries = 0; sinks = 0
    if (source=='ars'):
        for edge in edges_dict:
            if edge["entry"]:
                edges[edge["name"]+"-ENT"] = [datacarriles_list.index(datacarril) for datacarril in edge["datacarril"]]
                edges_entries.append(edge["name"]+"-ENT")
                entries += 1
            if not edge["entry"]:
                edges[edge["name"]+"-SAL"] = [datacarriles_list.index(datacarril) for datacarril in edge["datacarril"]]
                edges_sinks.append(edge["name"]+"-SAL")
                sinks += 1
    elif (source=='flexi'):
        for edge in edges_dict:
            if edge["entry"]:
                edges[edge["name"]] = [datacarriles_list.index(datacarril) for datacarril in edge["datacarril"]]
                edges_entries.append(edge["name"])
                entries += 1
            if not edge["entry"]:
                edges[edge["name"]] = [datacarriles_list.index(datacarril) for datacarril in edge["datacarril"]]
                edges_sinks.append(edge["name"])
                sinks += 1
    edges_order = edges_entries + edges_sinks
    origins = {}
    destinies = {}
    Fs = []
    for route in routes_dict:
        if not route["origin"] in origins:
            origins[route["origin"]] = [route["destiny"]]
        else: 
            origins[route["origin"]].append(route["destiny"])

        if not route["destiny"] in destinies:
            destinies[route["destiny"]] = [route["origin"]]
        else: 
            destinies[route["destiny"]].append(route["origin"])
        Fs.append(route["sink_rate"])


    cumfv = np.sum(fv[:,:len(datacarriles_list)], axis=1)
    zippedfv = list(zip(cumfv, range(8)))
    zippedorder = np.array(sorted(zippedfv, key = lambda x: x[0]), dtype=int)
    order = zippedorder[:,1].tolist()
    Fr = np.zeros([8, len(routes_dict)])
    counter = 0
    for k in order:
        fvC = np.array(fv)[k,:]
        M = np.array([np.sum(fvC[edges[edge]]) for edge in edges_order])
        M = np.append(M, np.sum(M[:entries]) - np.sum(M[sinks:]))

        columns = []
        rE = []
        entries_matrix = np.zeros([entries + sinks + 1, len(routes_dict) + entries])
        nphi = 0
        nentry = 0
        for entry in origins:
            for sink in origins[entry]:
                columns.append(entry + '=>' + sink)
                entries_matrix[nentry, nphi] = M[edges_order.index(entry+"-ENT")] if source == 'ars' else M[edges_order.index(sink)]
                if source=='ars':
                    rE.append(M[edges_order.index(sink+"-SAL")])
                elif source=='flexi':
                    rE.append(M[edges_order.index(sink)])
                nphi += 1
            entries_matrix[nentry, entries + sinks + nentry] = M[-1]
            nentry += 1
        ones_entries = np.clip(np.abs(entries_matrix), 0, 1)

        sinks_matrix = np.zeros([entries + sinks + 1, len(routes_dict) + entries])
        nphi = 0
        nsink = 0
        for sink in destinies:
            for entry in destinies[sink]:
                sinks_matrix[entries + nsink, nphi] = M[edges_order.index(sink+"-SAL")] if source == 'ars' else M[edges_order.index(sink)]
                nphi += 1
            sinks_matrix[-1, entries + sinks + nsink] = M[-1]
            nsink += 1
        ones_sinks = np.clip(np.abs(sinks_matrix), 0, 1)
        
        L_matrix = np.vstack([np.hstack([ones_entries, ones_sinks]), np.hstack([sinks_matrix, entries_matrix])])

        M = np.hstack([np.ones(entries + sinks + 1), M])
        phi_r = np.linalg.pinv(L_matrix) @ M.reshape([(entries + sinks + 1)*2, 1])
        Fr[counter, :] = np.ceil(np.array(rE) * phi_r[:len(routes_dict)].T)
        counter += 1

    cmap = matplotlib.cm.get_cmap('jet', 8)
    fig = plt.figure(figsize=(16.0, 12.0))
    fig.subplots_adjust(bottom=0.3)
    ap1 = fig.add_subplot(111)
    for i in range(8):
        ap1.plot(Fr[i,:],'-o', c=cmap(i), linewidth=2)
    ap1.set_xticks(range(len(columns)))
    ap1.set_xticklabels(columns, fontsize=9, rotation=30)
    ap1.set_ylabel('Vehicular flow\n[ Veh. class1 / 5 mins ]')
    ap1.grid()
    labelnames = ["C0", "C1", "C2", "C3", "C4", "C5", "C6", "C7"]
    fig.legend(ap1, labels=labelnames, loc="center right", borderaxespad=0.5, title="Cluster ID")
    fig.savefig(os.path.join(workspace_dir,"routes_demand.png"))
    plt.close()
    print("[pyinfmostro {}] routes demand graphical resultas added to workspace.".format(xsection_name))

    return Fr, Fs