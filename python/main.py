import sys
import yaml
import tkinter as tk
from tkinter import filedialog
from pyinfmostro.historic_data import preprocess_data
from pyinfmostro.svd_kmeans import svd_kmeans
from pyinfmostro.svd_kmeans import plot_results
from pyinfmostro.least_squares import routes_flow
from pyinfmostro.optimize import opt_plans

base = tk.Tk()
base.withdraw()

yml_filepath = filedialog.askopenfilename()

with open(yml_filepath, 'rb') as yml_file:
    conf = yaml.load(yml_file, Loader=yaml.FullLoader)

xsection_name = conf['xsection.name']
workspace_dir = conf['xsection.workspace']
datacarril_list = []
conf_edges = conf['xsection.edges']
for edge in conf_edges:
    datacarril_list += edge['datacarril']
conf_routes = conf['xsection.routes']
conf_phases = conf['xsection.phases']

if 'append_historical' in conf:
    host = conf['append_historical']['host']
    start_date = conf['append_historical']['start_date']
    end_date = conf['append_historical']['end_date']
    preprocess_data(host, start_date, end_date, xsection_name, datacarril_list, workspace_dir)
else:
    print('[pyinfmostro {}] skipping append data, working with {} as it is.'.format(xsection_name, '{}{}.csv'.format(workspace_dir,xsection_name)))

x, s, vh, means, labels, clabels, fv = svd_kmeans(xsection_name, datacarril_list, workspace_dir)
plot_results(xsection_name, workspace_dir, x, s, vh, means, labels, clabels, fv)
Fr, Fs = routes_flow(xsection_name, workspace_dir, fv, datacarril_list, conf_edges, conf_routes)
opt_plans(xsection_name, Fr, Fs, conf_routes, conf_phases, workspace_dir)