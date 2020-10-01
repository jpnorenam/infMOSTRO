import os
import time
import urllib3
import json
import datetime as dt
import pandas as pd
import numpy as np
from elasticsearch import Elasticsearch
from pandasticsearch import Select

def query_formatter(host, datacarril_list, start_mktime, end_mktime):
    carriles_query = []
    [carriles_query.append('{%22match_phrase%22:%20{%22Data.Carril%22:%20%22'+ id_str.replace('\n','').replace(' ','%20') + '%22}},') for id_str in datacarril_list]
    query_url = (
            host +'/consultar?indice=simmtrafficdata&tipo=doc&query='
            '{%22version%22:%20true,%22size%22:%20500,%22sort%22:%20'
            '[{%22Data.FechaHora%22:%20{%22order%22:%20%22desc%22,%22unmapped_type%22:%20%22boolean%22}}],'
            '%22_source%22:%20{%22excludes%22:%20[]},'
            '%22aggs%22:%20{%222%22:%20{%22date_histogram%22:%20{%22field%22:%20%22Data.FechaHora%22,%22fixed_interval%22:%20%2230s%22,%22time_zone%22:%20%22America/Bogota%22,%22min_doc_count%22:%201}}},'
            '%22stored_fields%22:%20[%22*%22],%22script_fields%22:%20{},%22docvalue_fields%22:%20[{%22field%22:%20%22@timestamp%22,%22format%22:%20%22date_time%22},'
            '{%22field%22:%20%22Data.FechaHora%22,%22format%22:%20%22date_time%22},{%22field%22:%20%22Indra.TimeStamp%22,%22format%22:%20%22date_time%22}],'
            '%22query%22:%20{%22bool%22:%20{%22must%22:%20[{%22match_all%22:%20{}},{%22bool%22:%20{%22should%22:%20'
            '[' + ''.join(carriles_query)[:-1] + '],%22minimum_should_match%22:%201}},'
            '{%22range%22:%20{%22Data.FechaHora%22:%20{%22gte%22:%20' + str(start_mktime) + ',%22lte%22:%20' + str(end_mktime) + ',%22format%22:%20%22epoch_millis%22}}}],'
            '%22filter%22:%20[],%22should%22:%20[],%22must_not%22:%20[]}}}')
    return query_url

def colapse_csv_ars(input_dir, datacarril_list, output_path):
    carril_flowlist = [carril + '.F' for carril in datacarril_list]
    carril_speedlist = [carril + '.S' for carril in datacarril_list]
    timeline = np.arange(0, 1440, 5)
    nbins = len(timeline)
    datetime_info = ['Index', 'Timestamp', 'Day', 'Hour']
    df = pd.DataFrame(columns = (datetime_info + carril_flowlist + carril_speedlist))
    df.set_index('Index')
    df['Index'] = range(nbins)
    for r, d, f in os.walk(input_dir):
        for file in f:
            with open(r + '/' + file) as json_file:
                json_data = json.load(json_file)
            json_data = json_data['hits'] if 'hits' in json_data else []
            for hit in json_data:
                data_ = hit['_source']['Data']
                timestamp_ = dt.datetime.strptime(data_['FechaHora'], "%Y-%m-%dT%H:%M:%S.%fZ") + dt.timedelta(hours=-10)
                bin_ = np.digitize(timestamp_.hour * 60 + timestamp_.minute, timeline)
                df.loc[bin_,'Timestamp'] = timestamp_.strftime("%Y-%m-%dT%H:%M")
                df.loc[bin_,'Day'] = timestamp_.weekday()
                df.loc[bin_,'Hour'] = timestamp_.hour + timestamp_.minute/60
                df.loc[bin_,data_['Carril']+'.F'] = data_['Clasificacion1'] + 2 * data_['Clasificacion2'] + 2.5 * data_['Clasificacion3']
                df.loc[bin_,data_['Carril']+'.S'] = data_['Velocidad']
    df = df.dropna(); del df["Index"]
    if not os.path.exists(os.path.dirname(output_path)):
        os.makedirs(os.path.dirname(output_path))
    if not os.path.exists(output_path):
        df.to_csv(output_path, sep=',', index=False, encoding='utf8')
    else:
        df.to_csv(output_path, sep=',', index=False, encoding='utf8', mode='a', header=False)

def timeline_progress(iteration, total, prefix = '', suffix = '', decimals = 1, length = 100, fill = '█', printEnd = "\r"):
    percent = ("{0:." + str(decimals) + "f}").format(100 * (iteration / float(total)))
    filledLength = int(length * iteration // total)
    bar = fill * filledLength + '-' * (length - filledLength)
    print('\r%s |%s| %s%% %s' % (prefix, bar, percent, suffix), end = printEnd)
    if iteration == total: 
        print()

def preprocess_data_ars(host, start_date, end_date, xsection_name, datacarril_list, workspace):
    csv_path = os.path.join(workspace, xsection_name + '.csv')
    datetime_0 = dt.datetime.strptime(start_date, '%Y-%m-%d')
    datetime_1 = dt.datetime.strptime(start_date, '%Y-%m-%d')
    datetime_end = dt.datetime.strptime(end_date, '%Y-%m-%d')
    init_date = datetime_0.day
    init_mktime = time.mktime(datetime_0.timetuple())
    total_timeline = time.mktime(datetime_end.timetuple()) - init_mktime

    http = urllib3.PoolManager()
    timeline_progress(0, total_timeline, prefix = '[pyinfmostro {}] timeline data preprocessed:'.format(xsection_name), suffix = 'completed', length = 50)
    while datetime_0 < datetime_end:
        datetime_1 = datetime_0 + dt.timedelta(minutes=5)
        mktime_0 = int(time.mktime(datetime_0.timetuple()))
        mktime_1 = int(time.mktime(datetime_1.timetuple()))
        res_path = 'tmp/{}/{}.json'.format(xsection_name, datetime_0.strftime('%Y/%m/%d/%H-%M'))

        if not os.path.exists(res_path):
            query = query_formatter(host, datacarril_list, mktime_0 * 1000, mktime_1 * 1000)
            res = json.loads(http.request('GET', query).data)
            if not os.path.exists(os.path.dirname(res_path)):
                os.makedirs(os.path.dirname(res_path))
            with open(res_path, 'w') as res_file:
                json.dump(res, res_file)

        if (datetime_1.hour != datetime_0.hour):
            timeline_progress(mktime_1 - init_mktime, total_timeline, prefix = '[pyinfmostro {}] timeline data preprocessed:'.format(xsection_name), suffix = 'completed', length = 50)
            if (datetime_1.strftime('%Y/%m/%d') != datetime_0.strftime('%Y/%m/%d')):
                input_dir = 'tmp/{}/{}'.format(xsection_name, datetime_0.strftime('%Y/%m/%d'))
                df_date = colapse_csv_ars(input_dir, datacarril_list, csv_path)
        datetime_0 = datetime_1
    print('[pyinfmostro {}] {} was correctly modified'.format(xsection_name, csv_path))

def preprocess_data_flexi(host, source, index, start_date, end_date, xsection_name, datacarril_list, workspace):
    csv_path = os.path.join(workspace, xsection_name + '.csv')
    datetime_0 = dt.datetime.strptime(start_date, '%Y-%m-%d')
    datetime_1 = dt.datetime.strptime(start_date, '%Y-%m-%d')
    datetime_end = dt.datetime.strptime(end_date, '%Y-%m-%d')
    init_date = datetime_0.day
    init_mktime = time.mktime(datetime_0.timetuple())
    total_timeline = time.mktime(datetime_end.timetuple()) - init_mktime
    
    es = Elasticsearch([host], timeout=600)
    timeline_progress(0, total_timeline, prefix = '[pyinfmostro {}] timeline data preprocessed:'.format(xsection_name), suffix = 'completed', length = 50)
    while datetime_0 < datetime_end:
        datetime_1 = datetime_0 + dt.timedelta(minutes=3600)
        res_path = 'tmp/{}/{}.json'.format(xsection_name, datetime_0.strftime('%Y/%m/%d/%H-%M'))
        if not os.path.exists(res_path):
            query = es.search(index=index, 
                        body = {
                            "query": {
                                    "bool": {
                                        "must": [               
                                            { "range":
                                                {"@timestamp": {
                                                    "time_zone": "-05:00",
                                                    "gte": datetime_0.strftime('%Y-%m-%dT%H:%M:%S'),
                                                    "lte": datetime_1.strftime('%Y-%m-%dT%H:%M:%S')
                                                    }
                                                }
                                            },
                                            { "match_phrase": { "junction": xsection_name } },
                                        ]
                                    }
                                }
                            }, size=10000)
            query = Select.from_dict(query).to_pandas()
            res = json.loads(http.request('GET', query).data)
            if not os.path.exists(os.path.dirname(res_path)):
                os.makedirs(os.path.dirname(res_path))
            with open(res_path, 'w') as res_file:
                json.dump(res, res_file)

        if (datetime_1.hour != datetime_0.hour):
            timeline_progress(mktime_1 - init_mktime, total_timeline, prefix = '[pyinfmostro {}] timeline data preprocessed:'.format(xsection_name), suffix = 'completed', length = 50)
            if (datetime_1.strftime('%Y/%m/%d') != datetime_0.strftime('%Y/%m/%d')):
                input_dir = 'tmp/{}/{}'.format(xsection_name, datetime_0.strftime('%Y/%m/%d'))
                df_date = colapse_csv(input_dir, datacarril_list, csv_path)
        datetime_0 = datetime_1
    print('[pyinfmostro {}] {} was correctly modified'.format(xsection_name, csv_path))
