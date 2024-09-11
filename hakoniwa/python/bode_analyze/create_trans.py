import json
import re
from sympy import symbols, Poly, simplify

class TransParser:
    def __init__(self, json_file):
        self.json_file = json_file
        self.data = self.load_transfer_function()
        self.constants = self.data['constants']

    def update_constant(self, const_name, new_value):
        if const_name in self.constants:
            self.constants[const_name] = new_value
        else:
            print(f"{const_name} is not found in constants")

    # JSONファイルの読み込み
    def load_transfer_function(self):
        with open(self.json_file, 'r') as file:
            data = json.load(file)
        return data

    # 伝達関数の各項を定数で置き換えて評価する
    def evaluate_terms(self, terms):
        evaluated_terms = []
        for term in terms:
            for const, value in self.constants.items():
                # 正規表現で変数名の前後が他の文字と一致しない場合のみ置き換え
                pattern = r'\b' + re.escape(const) + r'\b'
                term = re.sub(pattern, str(value), term)
            evaluated_terms.append(eval(term))  # 式を計算
        return evaluated_terms

    # C(s) の取得
    def get_controller(self):
        controller_data = self.data['controller']
        numerator = self.evaluate_terms(controller_data['num'])
        denominator = self.evaluate_terms(controller_data['den'])
        return Poly(numerator, s), Poly(denominator, s)

    # P(s) (プラント) の取得
    def get_plants(self):
        plants_data = self.data['plants']
        overall_num = Poly([1], s)  # 初期値 1
        overall_den = Poly([1], s)  # 初期値 1
        for plant in plants_data:
            num = self.evaluate_terms(plant['num'])
            den = self.evaluate_terms(plant['den'])
            plant_num_poly = Poly(num, s)
            plant_den_poly = Poly(den, s)
            # 複数プラントがある場合は掛け合わせる
            overall_num *= plant_num_poly
            overall_den *= plant_den_poly
        return overall_num, overall_den

    # L(s) = C(s) * P(s) の計算
    def calculate_l(self):
        # コントローラ C(s) とプラント P(s) を取得
        C_num, C_den = self.get_controller()
        P_num, P_den = self.get_plants()

        # L(s) の計算 (分子同士と分母同士の掛け算)
        L_num = C_num * P_num
        L_den = C_den * P_den
        return L_num, L_den

    # W(s) = L(s) / (1 + L(s)) の計算
    # W(s) = L_num / (L_den + L_num) の計算
    def calculate_w(self):
        # L(s) の分子と分母を取得
        L_num, L_den = self.calculate_l()

        # W(s) = L_num / (L_den + L_num) を計算
        W_num = L_num  # W(s) の分子は L(s) の分子
        W_den = L_den + L_num  # W(s) の分母は L(s) の分母 + L(s) の分子

        return W_num,  W_den
    
    # Ed(s) = P(s) / (1 + L(s)) の計算
    def calculate_ed(self):
        # プラント P(s) を取得
        P_num, P_den = self.get_plants()
        
        # L(s) の分子と分母を取得
        L_num, L_den = self.calculate_l()

        # Ed(s) = P_num / (P_den * (1 + L(s)))
        Ed_num = P_num  # Ed(s) の分子はプラント P(s) の分子
        Ed_den = P_den * (L_den + L_num)  # 分母は P(s) の分母と 1 + L(s) の掛け算
        
        return Ed_num, Ed_den

# s をシンボルとして定義
s = symbols('s')

# 使用例
json_file = 'python/bode_analyze/trans.json'
parser = TransParser(json_file)

# W(s) の計算
W_num, W_den = parser.calculate_w()
print("W(s) の分子:", W_num)
print("W(s) の分母:", W_den)

# L(s) の計算
L_num, L_den = parser.calculate_l()
print("L(s) の分子:", L_num)
print("L(s) の分母:", L_den)

# Ed(s) の計算
Ed_num, Ed_den = parser.calculate_ed()
print("Ed(s) の分子:", Ed_num)
print("Ed(s) の分母:", Ed_den)
