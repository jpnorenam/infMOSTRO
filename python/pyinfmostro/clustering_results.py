
import os
import sys
import numpy as np
import pandas as pd
import matplotlib
print(matplotlib.get_backend())
matplotlib.rcParams.update({'font.size': 16})
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D
import pickle
import json

xsection = str(sys.argv[1])
source = str(sys.argv[2])

cmap_name = 'jet'


s = pickle.load(open(os.path.join(datadir, 's.pickle'), 'rb'))
vh = pickle.load(open(os.path.join(datadir, 'vh.pickle'), 'rb'))
cum_energy = np.cumsum(s*s/np.sum(s*s))
ars = int(len(s)/2)

with open(os.path.join(datadir, xsection + '.json')) as json_file:
    json_data = json.load(json_file)

for k in range(3):
    sv_str = 'sv{0}'.format(str(k+1))
    json_data['singular_vectors'][k][sv_str] = vh[k,:].tolist()

fig = plt.figure(figsize=(16.0, 12.0))
plt.plot(np.arange(1, len(cum_energy)+1, 1), cum_energy * 100,'-o', linewidth=3)
plt.xlim([1, len(cum_energy)])
#plt.title('Singular values cumulative sum [ '+ xsection + ' ]')
plt.ylabel('Percentage of variation explained [ % ]')
plt.xlabel('Number of PCA components')
plt.grid()
fig.savefig(os.path.join(datadir,"svdenergy_{}.png".format(xsection)))
plt.close()

x = pickle.load(open(os.path.join(datadir, 'x.pickle'), 'rb'))
means = pickle.load(open(os.path.join(datadir, 'means.pickle'), 'rb'))
labels = pickle.load(open(os.path.join(datadir, 'labels.pickle'), 'rb'))
clabels = np.unique(labels)

for k in range(len(means)):
    json_data['clusters'][k]['centroid'] = means[k,:].tolist()

with open(os.path.join(datadir, xsection + '.json'), 'w') as outfile:
    json.dump(json_data, outfile)

#fv = means@np.linalg.pinv(vh[0:3,:].T)
fv = pickle.load(open(os.path.join(datadir, 'fv.pickle'), 'rb'))
if (source == 'ars'):
    cumfv = np.sum(fv[:,:ars], axis=1)
elif (source == 'flexi'):
    cumfv = np.sum(fv[:,:flexi_ent+flexi_sal], axis=1)
zippedfv = list(zip(cumfv, clabels))
zippedorder = np.array(sorted(zippedfv, key = lambda x: x[0]), dtype=int)
order = zippedorder[:,1].tolist()
labelnames = ["Cluster_1", "Cluster_2", "Cluster_3", "Cluster_4", "Cluster_5", "Cluster_6", "Cluster_7", "Cluster_8"]
labelorder = [labelnames[order.index(x)] for x in clabels]

cmap = matplotlib.cm.get_cmap(cmap_name, len(clabels))
cluster_lc = [cmap(order.index(x)) for x in labels]
means_lc = [cmap(order.index(x)) for x in clabels]

fig = plt.figure(figsize=(16.0, 12.0))
ax = fig.add_subplot(111, projection='3d')
#ax.title.set_text('Clustering [ '+ xsection + ' ]')
ax.set_xlabel('PCA component 1', labelpad=20)
ax.set_ylabel('PCA component 2', labelpad=20)
ax.set_zlabel('PCA component 3', labelpad=20)
ax_scatter = ax.scatter(x[:,0], x[:,1], x[:,2], c=cluster_lc)
#fig.legend([ax], labels=clabels, loc="center right", borderaxespad=0.5, title="Cluster")
fig.savefig(os.path.join(datadir,"clustering_{}.png".format(xsection)))
plt.close()

if(source == "ars"):
    fig = plt.figure(figsize=(16.0, 12.0))
    ap1 = fig.add_subplot(211)
    j = 0
    for i in clabels:
        #print(fv[i,0:ars])
        ap1.plot(fv[i,0:ars],'-o', c=means_lc[j], linewidth=2)
        j+=1
    ap1.set_xlabel('ARS lane')
    ap1.set_ylabel('Vehicular flow\n[ Veh class1 / 5 mins ]')
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
    fig.savefig(os.path.join(datadir,"profiles_{}.png".format(xsection)))
    plt.close()

elif(source == "flexi"):
    fig = plt.figure(figsize=(16.0, 12.0))
    ap1 = fig.add_subplot(311)
    j = 0
    for i in clabels:
        print(fv[i,0:len(flexi_f)])
        ap1.plot(fv[i,0:flexi_ent+flexi_sal]/12,'-o', c=means_lc[j], linewidth=2)
        j+=1
    ap1.set_xticks(range(len(flexi_f)))
    ap1.set_xticklabels(flexi_f, fontsize=9)
    ap1.set_ylabel('Vehicular flow\n[ Veh class1 / 5 mins ]')
    ap1.grid()
    ap2 = fig.add_subplot(312)
    j = 0
    for i in clabels:
        ap2.plot(fv[i,len(flexi_f):len(flexi_f)+len(flexi_v)],'-o', c=means_lc[j], linewidth=2)
        j+=1
    ap2.set_xticks(range(len(flexi_v)))
    ap2.set_xticklabels(flexi_v, fontsize=9)
    ap2.set_ylabel('Average speed\n[ km/h ]')
    ap2.grid()
    ap3 = fig.add_subplot(313)
    j = 0
    for i in clabels:
        ap3.plot(fv[i,len(flexi_f)+len(flexi_v):],'-o', c=means_lc[j], linewidth=2)
        j+=1
    ap3.set_xticks(range(len(flexi_q)))
    ap3.set_xticklabels(flexi_q, fontsize=9)
    ap3.set_ylabel('Queue [ m ]')
    ap3.grid()
    fig.legend([ap1, ap2, ap3], labels=labelorder, loc="center right", borderaxespad=0.5, title="Cluster ID")
    fig.savefig(os.path.join(datadir,"profiles_{}.png".format(xsection)))
    plt.close()

dh = pd.read_csv(os.path.join(datadir, xsection + '_labeled.csv'))

fig = plt.figure(figsize=(16.0, 12.0))
ax1 = fig.add_subplot(211)
h1 = ax1.hist([dh.loc[(dh.Label == x) & ((dh.Day == 0) | (dh.Day == 1) | (dh.Day == 2) |  (dh.Day == 3) | (dh.Day == 4)), 'Hour'] for x in clabels], bins=24, color=means_lc, density=False, label=clabels)
ax1.title.set_text('[ '+ xsection + ' ] : Lu, Ma, Mi, Ju, Vi')
ax1.set_xlim([0, 24])
ax1.set_ylabel('Frequency')
ax1.set_xlabel('Hour')
ax2 = fig.add_subplot(212)
h2 = ax2.hist([dh.loc[(dh.Label == x) & ((dh.Day == 5) | (dh.Day == 6)), 'Hour'] for x in clabels], bins=24, color=means_lc, density=False, label=clabels)
ax2.title.set_text('[ '+ xsection + ' ] : Sa, Do')
ax2.set_xlim([0, 24])
ax2.set_ylabel('Frequency')
ax2.set_xlabel('Hour')
fig.legend([h1, h2], labels=labelorder, loc="center right", borderaxespad=0.5, title="Cluster ID")
fig.savefig(os.path.join(datadir,"hourlydist_{}.png".format(xsection)))
plt.close()
