from skmultilearn.dataset import load_dataset
import numpy as np
import itertools
from scipy.stats import chi2_contingency

def calculate_metrics(X, y):
    n, f = X.shape  # Número de instancias y atributos
    l = y.shape[1]  # Número de etiquetas
    
    # Cardinalidad
    card = np.mean(np.sum(y.toarray(), axis=1))
    
    # Densidad
    den = card / l
    
    # Diversidad
    unique_labelsets = {tuple(row) for row in y.toarray()}
    total_possible_labelsets = 2 ** l
    div = len(unique_labelsets) / total_possible_labelsets
    
    # Imbalance Ratio
    label_frequencies = np.sum(y.toarray(), axis=0)
    max_freq = np.max(label_frequencies)
    IR_per_label = max_freq / (label_frequencies + 1e-6)  # Para evitar división por cero
    avgIR = np.mean(IR_per_label)
    
    # Dependencia de etiquetas por Chi-cuadrado
    label_matrix = y.toarray()
    dependent_pairs = 0
    total_pairs = 0
    
    for i, j in itertools.combinations(range(l), 2):
        contingency_table = np.zeros((2, 2))
        contingency_table[0, 0] = np.sum((label_matrix[:, i] == 0) & (label_matrix[:, j] == 0))
        contingency_table[0, 1] = np.sum((label_matrix[:, i] == 0) & (label_matrix[:, j] == 1))
        contingency_table[1, 0] = np.sum((label_matrix[:, i] == 1) & (label_matrix[:, j] == 0))
        contingency_table[1, 1] = np.sum((label_matrix[:, i] == 1) & (label_matrix[:, j] == 1))
        
        if np.any(contingency_table == 0):
            continue  # Evitar cálculos con tablas con ceros
        
        _, p_value, _, _ = chi2_contingency(contingency_table)
        if p_value < 0.01:  # 99% de confianza
            dependent_pairs += 1
        total_pairs += 1
    
    rDep = dependent_pairs / total_pairs if total_pairs > 0 else 0
    
    return {
        "instances (n)": n,
        "attributes (f)": f,
        "labels (l)": l,
        "cardinality (car)": card,
        "density (den)": den,
        "diversity (div)": div,
        "average Imbalance Ratio (avgIR)": avgIR,
        "ratio of dependent labels (rDep)": rDep
    }

datasets = ['scene', 'Corel5k', 'bibtex', 'enron', 'rcv1subset5', 'tmc2007_500', 'rcv1subset3',
            'rcv1subset1', 'delicious', 'rcv1subset4', 'genbase', 'birds', 'emotions',
            'rcv1subset2', 'mediamill', 'medical', 'yeast']

def main():
    results = {}
    for dataset in datasets:
        try:
            (X_train, y_train, _, _), (X_test, y_test, _, _) = load_dataset(dataset, "train"), load_dataset(dataset, "test")
            metrics = calculate_metrics(X_train, y_train)
            results[dataset] = metrics
            print(f"Dataset: {dataset}")
            for key, value in metrics.items():
                print(f"  {key}: {value:.4f}")
            print("-" * 50)
        except Exception as e:
            print(f"Error loading dataset {dataset}: {e}")
            if "zero element" in str(e):
                print(f"Skipping dataset {dataset} due to zero element error in chi-square test.")

if __name__ == "__main__":
    main()
