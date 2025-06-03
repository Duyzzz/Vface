import csv
import pandas as pd
import numpy as np
from tkinter import Tk
from tkinter.filedialog import asksaveasfilename
from datetime import datetime
from matplotlib.backends.backend_pdf import PdfPages
import matplotlib.pyplot as plt
import path_str as pathStr
FILE_PATH = pathStr.HISTORY_FILE_PATH
def append(row):
    with open(FILE_PATH, mode="a", newline="") as file:
        writer = csv.writer(file)
        writer.writerow(row)

def convertToExcel():
    # Hide the root Tkinter window
    root = Tk()
    root.withdraw()

    # Ask the user to select a location to save the file
    save_path = asksaveasfilename(
        defaultextension=".xlsx",
        filetypes=[("Excel files", "*.xlsx")],
        title="Save as"
    )

    if save_path:  # If the user selects a path
        df = pd.read_csv(FILE_PATH)
        df.to_excel(save_path, index=False)

def filDataByDays(startDay, endDay):
    print(f"filter data with start day: {startDay}, type:{type(startDay)}, end day {endDay}, type:{type(endDay)}")
    start_date = datetime.strptime(startDay, "%d-%m-%Y")
    end_date = datetime.strptime(endDay,  "%d-%m-%Y")
    try:
        df = pd.read_csv(FILE_PATH)
        df.columns = df.columns.str.strip()  # Remove leading/trailing spaces from column names
        df["date"] = pd.to_datetime(df["date"], format="%d-%m-%Y")  # Convert 'date' column to datetime
        mask = (df["date"] >= start_date) & (df["date"] <= end_date)
        filtered_df = df.loc[mask]
        print(f"Filtered data:\n{filtered_df}")
        # Save the filtered data to a new CSV file
        root = Tk()
        root.withdraw()
        save_path = asksaveasfilename(
            defaultextension=".xlsx",
            filetypes=[("Excel files", "*.xlsx")],
            title="Save as"
        )
        if save_path:  # If the user selects a path
            drawGraphsAndSaveToPDF(filtered_df, save_path + ".pdf")
            filtered_df.to_excel(save_path, index=False)
            print(f"Filtered data saved to {save_path}")
        else:
            # If the user cancels the save dialog, save to a default location
            filtered_df.to_excel("reports\\filtered_data.csv", index=False)
            drawGraphsAndSaveToPDF(filtered_df, "reports\\filtered_data.csv" + ".pdf")
            print("Filtered data saved to reports\\filtered_data.csv")
    except ValueError as e:
        print(f"Error: {e}. Ensure the 'date' column exists in the CSV file.")
        return
    
def drawGraphsAndSaveToPDF(df_data, save_path):
    # Draw bar chart by name and number of occurrences
    numberOfOccurrencesByName = {}
    for index, row in df_data.iterrows():
        name = row["Name"]
        if name in numberOfOccurrencesByName:
            numberOfOccurrencesByName[name] += 1
        else:
            numberOfOccurrencesByName[name] = 1
    print(f"numberOfOccurrencesByName: {numberOfOccurrencesByName}")
    
    # Sort the dictionary by keys (names) to ensure consistent order
    sorted_names = sorted(numberOfOccurrencesByName.keys())
    sorted_values = [numberOfOccurrencesByName[name] for name in sorted_names]
    # x = np.arange(len(sorted_names))  # Set x locations for the bars
    # numberOfPeople = len(sorted_names)

    # Draw bar chart
    plt.figure(figsize=(10, 6))
    bar_width = 0.2  # Set a fixed width for the bars
    plt.bar(sorted_names, sorted_values, width=bar_width, color='skyblue', edgecolor='black', align='center')
    plt.xlabel("Name")
    plt.ylabel("Number of Occurrences")
    plt.title("Number of Occurrences by Name")
    plt.xticks(rotation=45)
    plt.tight_layout()
    
    # Save the bar chart to a PDF file
    with PdfPages(save_path) as pdf:
        plt.savefig(pdf, format='pdf')
        plt.close()
