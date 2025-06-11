import numpy as np
import itertools
import scipy.io
from scipy.stats import chi2_contingency
from scipy.sparse import csr_matrix
from scipy.io.arff import loadarff
import os

def load_custom_dataset(name, base_path):
    file_path = os.path.join(base_path, name)
    file_ext = os.path.splitext(file_path)[1].lower()

    if file_ext == ".mat":
        mat_data = scipy.io.loadmat(file_path)
        X = mat_data['data']
        y = mat_data['targets']
    elif file_ext == ".arff":
        with open(file_path, "r") as f:
            data, meta = loadarff(f)

        # Convert structured array to numpy array
        X = np.array([list(row)[:-1] for row in data], dtype=np.float32)

        # Handling categorical labels correctly
        y_raw = np.array([row[-1] for row in data])
        if y_raw.dtype.char == 'S':  # If labels are in bytes (strings)
            unique_labels = np.unique(y_raw)
            label_map = {label: idx for idx, label in enumerate(unique_labels)}
            y = np.array([[1 if row[-1] == label else 0 for label in unique_labels] for row in data], dtype=np.int32)
        else:
            y = np.array([list(row)[-1:] for row in data], dtype=np.int32)

    else:
        raise ValueError("Unsupported file format")

    return csr_matrix(X), csr_matrix(y)

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

def main():
    base_path = "C:/Users/Usuario/OneDrive/Master/CNC/Practica1Multiclase/"
    datasets = ["CHD_49.mat", "cs.arff"]

    for dataset in datasets:
        try:
            X, y = load_custom_dataset(dataset, base_path)

            if X.shape[0] == 0 or y.shape[0] == 0:
                raise ValueError(f"Dataset {dataset} has zero instances.")

            metrics = calculate_metrics(X, y)
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
