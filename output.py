import pandas as pd

# Prepare structured data from the logs
data = [
    {
        "Frames": 10000,
        "Samples": 10000,
        "Min RTT (ms)": 0.108,
        "Max RTT (ms)": 2.816,
        "Average RTT (ms)": 0.154,
        "Histogram": {
            "0-1 ms": 9999,
            "2-3 ms": 1
        }
    },
    {
        "Frames": 100000,
        "Samples": 100000,
        "Min RTT (ms)": 0.107,
        "Max RTT (ms)": 3.241,
        "Average RTT (ms)": 0.158,
        "Histogram": {
            "0-1 ms": 99994,
            "1-2 ms": 5,
            "3-4 ms": 1
        }
    },
    {
        "Frames": 1000000,
        "Samples": 1000000,
        "Min RTT (ms)": 0.106,
        "Max RTT (ms)": 4.744,
        "Average RTT (ms)": 0.154,
        "Histogram": {
            "0-1 ms": 999986,
            "1-2 ms": 7,
            "2-3 ms": 5,
            "3-4 ms": 1,
            "4-5 ms": 1
        }
    },
    {
        "Frames": 10000000,
        "Samples": 10000000,
        "Min RTT (ms)": 0.104,
        "Max RTT (ms)": 20.132,
        "Average RTT (ms)": 0.161,
        "Histogram": {
            "0-1 ms": 9999736,
            "1-2 ms": 174,
            "2-3 ms": 29,
            "3-4 ms": 17,
            "4-5 ms": 17,
            "5-6 ms": 13,
            "6-7 ms": 4,
            "7-8 ms": 4,
            "8-9 ms": 1,
            "9-10 ms": 1,
            "10-11 ms": 2,
            "18-19 ms": 1,
            "20-21 ms": 1
        }
    }
]

# Flatten summary stats
summary = pd.DataFrame([{
    "Frames": d["Frames"],
    "Samples": d["Samples"],
    "Min RTT (ms)": d["Min RTT (ms)"],
    "Max RTT (ms)": d["Max RTT (ms)"],
    "Average RTT (ms)": d["Average RTT (ms)"]
} for d in data])

# Flatten histograms for each dataset
hist_dfs = []
for d in data:
    hist_df = pd.DataFrame(list(d["Histogram"].items()), columns=["Bucket (ms)", "Count"])
    hist_df.insert(0, "Frames", d["Frames"])
    hist_dfs.append(hist_df)

hist_all = pd.concat(hist_dfs, ignore_index=True)

# Save to Excel with two sheets
output_file = "/mnt/d/rudp_results.xlsx"
with pd.ExcelWriter(output_file, engine="openpyxl") as writer:
    summary.to_excel(writer, sheet_name="Summary", index=False)
    hist_all.to_excel(writer, sheet_name="Histograms", index=False)

output_file
