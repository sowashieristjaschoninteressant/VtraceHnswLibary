import pandas as pd
import matplotlib.pyplot as plt
import seaborn as sns




def main():
 datasets = ["siftsmall","gist", "sift"]
 libs = ["hnswlib", "vtrace"]


 colors = {"hnswlib":"blue", "faiss":"red", "vtrace":"green"}
 markers = {"hnswlib":"o", "faiss":"s", "vtrace":"^"}
 i = 1
 plt.figure(figsize=(24,6))
 for dataset in datasets:
    

    data={}
    for lib in libs:
       path=f"./csv/{lib}/{lib}_benchmark_insertion_{dataset}.csv"
       print(f"load: {path}")
       data[lib] = pd.read_csv(path)
 
    plt.subplot(1,3,i)
    for lib in libs:
       plt.plot(data[lib]['ef_construction'],data[lib]['avg_insert_us'], marker=markers[lib],color=colors[lib], label=lib)
          
    plt.xlabel("ef_construction")
    plt.ylabel("avg latency (us)")
    plt.title(f"{dataset} - Latency")
    plt.grid(True, linestyle='--', alpha=0.5)
    plt.legend()

    i = i + 1
 plt.tight_layout()
 plt.show()

 return







if __name__ == "__main__":
    main()