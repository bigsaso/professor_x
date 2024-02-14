import pandas as pd

def label(file, label):
    df = pd.read_csv(f"{file}.csv")
    df['Movement'] = label
    df.to_csv(f'{file}_labelled.csv', index=False)

if __name__ == '__main__':
    label("bothhands", 1)