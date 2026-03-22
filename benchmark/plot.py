#!/usr/bin/env python3
import pandas as pd
import matplotlib.pyplot as plt 
import seaborn as sns

def main():
    df = pd.read_csv("naive_benchmark_search.csv")
    print(df.head())
    plt.figure(figsize=(8,6))
    plt.plot(df['N'], df['search_time_ms'], marker='o')
    plt.xlabel("Number of nodes (N)")
    plt.ylabel("search time (s)")
    plt.title("HNSW search Time vs Number of Nodes")
    plt.grid(True)
    plt.show()

    return









if __name__ == "__main__":
    main()


