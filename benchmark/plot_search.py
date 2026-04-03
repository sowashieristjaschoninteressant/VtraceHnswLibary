#!/usr/bin/env python3
import pandas as pd
import matplotlib.pyplot as plt 
import seaborn as sns

def search_plot(datasets, libs, colors, markers):
   for dataset in datasets:
    plt.figure(figsize=(12, 4))
    
    
    data = {}
    for lib in libs:
        if lib == "faiss" and dataset == 'gist':
           continue
        path=f"./csv/{lib}/{lib}_benchmark_search_{dataset}.csv"
        print(f"load: {path}")
        data[lib] = pd.read_csv(path)
    
   
    plt.subplot(1,3,1)
    for lib in libs:
        if lib == "faiss" and dataset == 'gist':
           continue
        plt.plot(data[lib]['ef_search'], data[lib]['avg_latency_ms'], 
                 marker=markers[lib], color=colors[lib], label=lib)
    plt.xlabel("ef_search")
    plt.ylabel("Avg Latency (ms)")
    plt.title(f"{dataset} - Latency")
    plt.grid(True, linestyle='--', alpha=0.5)
    plt.legend()
    
   
    plt.subplot(1,3,2)
    for lib in libs:
        if lib == "faiss" and dataset == 'gist':
           continue
        plt.plot(data[lib]['ef_search'], data[lib]['recall'], 
                 marker=markers[lib], color=colors[lib], label=lib)
    plt.xlabel("ef_search")
    plt.ylabel("Recall")
    plt.title(f"{dataset} - Recall")
    plt.grid(True, linestyle='--', alpha=0.5)
    
   
    plt.subplot(1,3,3)
    for lib in libs:
        if lib == "faiss" and dataset == 'gist':
           continue
        plt.plot(data[lib]['ef_search'], data[lib]['qps'], 
                 marker=markers[lib], color=colors[lib], label=lib)
    plt.xlabel("ef_search")
    plt.ylabel("QPS")
    plt.title(f"{dataset} - QPS")
    plt.grid(True, linestyle='--', alpha=0.5)
    
    plt.tight_layout()
    plt.show()

def recall_QPS(datasets, libs,colors, markers):
 plt.figure(figsize=(18,5))

 for i, dataset in enumerate(datasets, start=1):
    data = {}
    for lib in libs:
        path = f"./csv/{lib}/{lib}_benchmark_search_{dataset}.csv"
        data[lib] = pd.read_csv(path)

    plt.subplot(1,3,i)
    for lib in libs:
        plt.plot(
            data[lib]['recall'],
            data[lib]['qps'],
            marker=markers[lib],
            color=colors[lib],
            label=lib
        )

    plt.xlabel("Recall")
    plt.ylabel("QPS")
    plt.title(f"{dataset} - Recall vs QPS")
    plt.grid(True, linestyle='--', alpha=0.5)
    plt.legend()

 plt.tight_layout()
 plt.show()
   
   

def main():
 datasets = ["sift", "siftsmall", "gist"]
 libs = ["hnswlib", "vtrace", "faiss"]

 
 colors = {"hnswlib":"blue", "faiss":"red", "vtrace":"green"}
 markers = {"hnswlib":"o", "faiss":"s", "vtrace":"^"}
 search_plot(datasets, libs, colors, markers)

 
 return

if __name__ == "__main__":
    main()


