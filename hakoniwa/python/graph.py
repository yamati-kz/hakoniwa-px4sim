import pandas as pd
import matplotlib.pyplot as plt

# CSVファイルの読み込み
df = pd.read_csv('height_data.csv')

# グラフの描画
plt.figure(figsize=(10, 6))  # グラフのサイズ指定
plt.plot(df['Time'], df['Height'])  # 時間に対する高さをプロット
plt.title('Height over Time')  # グラフのタイトル
plt.xlabel('Time')  # x軸のラベル
plt.ylabel('Height')  # y軸のラベル
plt.grid(True)  # グリッドの表示
plt.show()  # グラフの表示