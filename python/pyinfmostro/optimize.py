import os
import scipy.optimize as opt
import numpy as np
import pandas as pd

tc_max = 120
tc_min = 120

def opt_plans(xsection_name, Fr, Fs, conf_routes, conf_phases, workspace_dir):
    H = np.zeros([len(conf_routes), len(conf_phases)])
    r = 0
    Nc = []
    for route in conf_routes:
        H[r, [phase -1 for phase in route["green_phases"]]] = 1
        Nc.append(route["lanes"])
        r += 1
    Hc = np.invert(H.astype(bool)).astype(int)

    x_min = [phase["min_value"] for phase in conf_phases]
    x_max = [phase["max_value"] for phase in conf_phases]
    phase_ids = [phase["id"] for phase in conf_phases]
    x0 = x_min

    phasetimes = np.zeros([8, len(conf_phases)])
    for clust in range(8):
        n_c = np.array(Nc)
        fe = Fr[clust, :].reshape(len(Fr[clust, :]), 1)
        fsm = ((np.array(Fs) * n_c)/ 12).reshape(len(Fr[clust, :]), 1)
        fe_ps = fe / 300
        fsm_ps = fsm / 300

        lb = x_min
        ub = x_max
        bounds = opt.Bounds(lb, ub)
        constA = np.ones(len(x0))
        constraints =  opt.LinearConstraint(constA, tc_min, tc_max)
        optsol = opt.minimize(fun, x0, bounds = bounds, constraints = constraints, method='SLSQP',
                              options={'maxiter': 1000, 'ftol': 1e-8, 'disp': False, 'eps': 0.00005},
                              args=(H, Hc, fe_ps, fsm_ps))
        xopt = optsol.x
        #x0 = xopt
        tc = np.sum(xopt)
        funval = optsol.fun
        phasetimes[clust, :] = xopt
    df = pd.DataFrame(phasetimes, columns = phase_ids)
    df.to_csv(os.path.join(workspace_dir,"phase_times.csv"))
    print("[pyinfmostro {}] writting optimal planning results to workspace.".format(xsection_name))

def fun(x, *args):
    x = np.atleast_2d(x).T
    t_c = (np.sum(x))
    alpha = 1.0
    #alpha = t_c * 0.004 + 0.52
    #val = (fe_ps*(Hc @ x)) / (alpha*fsm_ps*(H @ x))
    val = (args[2]*(args[1] @ x)) / (alpha*args[3]*(args[0] @ x))
    return np.max(val)
