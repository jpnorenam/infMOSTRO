import os
import numpy as np
import pandas as pd
import sklearn.datasets
import sklearn.cluster
import scipy.linalg as linalg
import matplotlib
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D
import warnings

k = 8
cmap_name = 'jet'
print(matplotlib.get_backend())
matplotlib.rcParams.update({'font.size': 16})
warnings.simplefilter("ignore", UserWarning)

def svd_kmeans(xsection_name, datacarril_list, workspace):
    n_ars = len(datacarril_list)
    csv_dir = os.path.join(workspace, xsection_name + '.csv')
    labeled_dir = os.path.join(workspace, xsection_name + '_labeled.csv')

    dfv = pd.read_csv(csv_dir)
    dh = pd.DataFrame(dfv[['Timestamp','Day','Hour']])
    del dfv['Timestamp']; del dfv['Day']; del dfv['Hour']; to_del = []
    for col in dfv.columns:
        if(np.isnan(dfv[col].values[0]) or np.isnan(dfv[col].values[-1])):
            if (np.isnan(dfv[col].mean())):
                print("[infmostro {}] warning: {} has no data (is full on nan), infmostro is dismissing it for clustering.".format(xsection_name,col))
                to_del.append(col)
            else:
               if(np.isnan(dfv[col].values[-1])):
                    dfv[col].values[-1] = dfv[col].mean()
               if(np.isnan(dfv[col].values[0])):
                    dfv[col].values[0] = dfv[col].mean()
    for ctd in to_del:
        del dfv[ctd]

    dfv = (dfv.ffill() + dfv.bfill()) / 2
    dfv = dfv.values

    # Calcular la descomposición en valores signulares 
    # https://blogs.oracle.com/r/using-svd-for-dimensionality-reduction
    print("[infmostro {}] reducing and clustering historical data.".format(xsection_name))
    u, s, vh = linalg.svd(dfv, full_matrices=True)
    x = dfv@vh[0:3,:].T
    kmeans = sklearn.cluster.KMeans(k, max_iter=400, random_state=0)
    kmeans.fit(x)
    means = kmeans.cluster_centers_
    labels = kmeans.labels_
    fv = means@vh[0:3,:] # fv = means@np.linalg.pinv(vh[0:3,:]) unitary matrix A.T = A^-1
    dh['Label'] = kmeans.labels_
    dh.to_csv(labeled_dir, sep=',')
    clabels = dh.Label.unique()
    return x, s, vh, means, labels, clabels, fv

def plot_results(xsection_name, workspace, x, s, vh, means, labels, clabels, fv):
    ars = int(len(s)/2)
    cumfv = np.sum(fv[:,:ars], axis=1)
    zippedfv = list(zip(cumfv, clabels))
    zippedorder = np.array(sorted(zippedfv, key = lambda x: x[0]), dtype=int)
    order = zippedorder[:,1].tolist()
    labelnames = ["C0", "C1", "C2", "C3", "C4", "C5", "C6", "C7"]
    labelorder = [labelnames[order.index(x)] for x in clabels]
    cmap = matplotlib.cm.get_cmap(cmap_name, len(clabels))
    cluster_lc = [cmap(order.index(x)) for x in labels]
    means_lc = [cmap(order.index(x)) for x in clabels]


    fig = plt.figure(figsize=(16.0, 12.0))
    ax = fig.add_subplot(111, projection='3d')
    #ax.title.set_text('Clustering [ '+ xsection_name + ' ]')
    ax.set_xlabel('PCA component 1', labelpad=20)
    ax.set_ylabel('PCA component 2', labelpad=20)
    ax.set_zlabel('PCA component 3', labelpad=20)
    ax_scatter = ax.scatter(x[:,0], x[:,1], x[:,2], c=cluster_lc)
    #fig.legend([ax], labels=clabels, loc="center right", borderaxespad=0.5, title="Cluster")
    fig.savefig(os.path.join(workspace,"clustering.png"))
    plt.close()


    fig = plt.figure(figsize=(16.0, 12.0))
    ap1 = fig.add_subplot(211)
    j = 0
    for i in clabels:
        #print(fv[i,0:ars])
        ap1.plot(fv[i,0:ars],'-o', c=means_lc[j], linewidth=2)
        j+=1
    ap1.set_xlabel('ars lane')
    ap1.set_ylabel('Vehicular flow\n[ Veh. class1 / 5 mins ]')
    ap1.grid()
    ap2 = fig.add_subplot(212)
    j = 0
    for i in clabels:
        ap2.plot(fv[i,ars:],'-o', c=means_lc[j], linewidth=2)
        j+=1
    ap2.set_xlabel('ARS lane')
    ap2.set_ylabel('Average speed\n[ km/h ]')
    ap2.grid()
    fig.legend([ap1, ap2], labels=labelorder, loc="center right", borderaxespad=0.5, title="Cluster ID")
    fig.savefig(os.path.join(workspace,"profiles.png"))
    plt.close()

    dh = pd.read_csv(os.path.join(workspace, xsection_name + '_labeled.csv'))

    fig = plt.figure(figsize=(16.0, 12.0))
    ax1 = fig.add_subplot(211)
    h1 = ax1.hist([dh.loc[(dh.Label == x) & ((dh.Day == 0) | (dh.Day == 1) | (dh.Day == 2) |  (dh.Day == 3) | (dh.Day == 4)), 'Hour'] for x in clabels], bins=24, color=means_lc, density=False, label=clabels)
    ax1.title.set_text('[ '+ xsection_name + ' ] : Lu, Ma, Mi, Ju, Vi')
    ax1.set_xlim([0, 24])
    ax1.set_ylabel('Frequency')
    ax1.set_xlabel('Hour')
    ax2 = fig.add_subplot(212)
    h2 = ax2.hist([dh.loc[(dh.Label == x) & ((dh.Day == 5) | (dh.Day == 6)), 'Hour'] for x in clabels], bins=24, color=means_lc, density=False, label=clabels)
    ax2.title.set_text('[ '+ xsection_name + ' ] : Sa, Do')
    ax2.set_xlim([0, 24])
    ax2.set_ylabel('Frequency')
    ax2.set_xlabel('Hour')
    fig.legend([h1, h2], labels=labelorder, loc="center right", borderaxespad=0.5, title="Cluster ID")
    fig.savefig(os.path.join(workspace,"hourlydist.png"))
    plt.close()
    print("[infmostro {}] clustering graphical results added to workspace.".format(xsection_name))