import numpy as np
import time
from skmultilearn.dataset import load_dataset
from skmultilearn.problem_transform import BinaryRelevance, ClassifierChain, LabelPowerset
from sklearn.naive_bayes import GaussianNB
from sklearn.tree import DecisionTreeClassifier
from sklearn.model_selection import cross_val_predict, KFold
from sklearn.metrics import accuracy_score, hamming_loss, precision_recall_fscore_support

''' Uso KFold para particionar los datasets y aplicar crossvalidación '''
def evaluate_model(classifier, X, y):
    kf = KFold(n_splits=5, shuffle=True, random_state=42)
    
    start_time = time.time()
    y_pred = cross_val_predict(classifier, X.toarray(), y.toarray(), cv=kf)
    end_time = time.time()
    
    execution_time = end_time - start_time
    
    accuracy = accuracy_score(y, y_pred)
    hamming = hamming_loss(y, y_pred)
    precision, recall, f1, _ = precision_recall_fscore_support(y, y_pred, average='macro')
    
    return accuracy, hamming, precision, recall, f1, execution_time

def main():
    datasets = ['scene', 'enron', 'medical', 'yeast', 'genbase']
    base_classifiers = [GaussianNB(), DecisionTreeClassifier()]
    
    for base_clf in base_classifiers:
        classifiers = {
            'BinaryRelevance': BinaryRelevance(base_clf),
            'ClassifierChain': ClassifierChain(base_clf),
            'LabelPowerset': LabelPowerset(base_clf)
        }
        
        print(f"\nProbando con clasificador base: {base_clf.__class__.__name__}")
        
        for dataset in datasets:
            X, y, _, _ = load_dataset(dataset, 'undivided')
            print(f"\nDataset: {dataset}")
            
            for name, clf in classifiers.items():
                print(f"Método: {name}")
                accuracy, hamming, precision, recall, f1, execution_time = evaluate_model(clf, X, y)
                print(f"-  Accuracy: {accuracy:.4f}")
                print(f"-  Hamming Loss: {hamming:.4f}")
                print(f"-  Precision: {precision:.4f}")
                print(f"-  Recall: {recall:.4f}")
                print(f"-  F1-score: {f1:.4f}")
                print(f"-  Tiempo de ejecución: {execution_time:.2f} segundos")
                print("\n")

if __name__ == "__main__":
    main()
