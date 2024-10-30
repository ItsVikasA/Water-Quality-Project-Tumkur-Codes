import pandas as pd
import streamlit as st
import seaborn as sns
import matplotlib.pyplot as plt
from sklearn.model_selection import train_test_split
from sklearn.preprocessing import StandardScaler
from sklearn.svm import SVC
from sklearn.metrics import accuracy_score, confusion_matrix

# Load the dataset
data = pd.read_csv(r'water_potability  CE.csv')

# Title of the dashboard
st.title('Water Potability Dashboard')

# Display the dataset
if st.checkbox('Show Dataset'):
    st.write(data)

# Pairplot
if st.checkbox('Show Pairplot'):
    st.subheader('Pairplot of Water Quality Features')
    fig = sns.pairplot(data, hue='Potability')
    st.pyplot(fig)

# Correlation Heatmap
if st.checkbox('Show Correlation Heatmap'):
    st.subheader('Correlation Heatmap')
    plt.figure(figsize=(10, 8))
    corr = data.corr()
    sns.heatmap(corr, annot=True, fmt=".2f", cmap='coolwarm')
    st.pyplot(plt)

# Histograms of Features
if st.checkbox('Show Histograms'):
    st.subheader('Histograms of Features')
    for column in data.columns[:-1]:  # Exclude the target column
        plt.figure()
        sns.histplot(data[column], bins=30, kde=True)
        plt.title(f'Histogram of {column}')
        st.pyplot(plt)

# Boxplots of Features by Potability
if st.checkbox('Show Boxplots'):
    st.subheader('Boxplots of Features by Potability')
    plt.figure(figsize=(15, 10))
    for i, column in enumerate(data.columns[:-1], 1):  # Exclude the target column
        plt.subplot(2, 3, i)
        sns.boxplot(x='Potability', y=column, data=data)
        plt.title(f'Boxplot of {column} by Potability')
    plt.tight_layout()
    st.pyplot(plt)

# Data Preprocessing
X = data[['ph', 'Hardness', 'Solids', 'Conductivity', 'Turbidity']]
y = data['Potability']
X.fillna(X.mean(), inplace=True)

# Train-Test Split
X_train, X_test, y_train, y_test = train_test_split(X, y, test_size=0.2, random_state=42)

# Feature Scaling
scaler = StandardScaler()
X_train_scaled = scaler.fit_transform(X_train)
X_test_scaled = scaler.transform(X_test)

# Train SVM Model
svm_classifier = SVC(kernel='rbf', C=1.0, gamma='scale')
svm_classifier.fit(X_train_scaled, y_train)

# Make Predictions
y_pred = svm_classifier.predict(X_test_scaled)

# Evaluation Metrics
accuracy = accuracy_score(y_test, y_pred)
conf_matrix = confusion_matrix(y_test, y_pred)

# Display Metrics
st.subheader('Model Evaluation Metrics')
st.write(f"Accuracy: {accuracy * 100:.2f}%")
st.write("Confusion Matrix:", conf_matrix)

# Confusion Matrix Visualization
if st.checkbox('Show Confusion Matrix'):
    st.subheader('Confusion Matrix')
    plt.figure(figsize=(6, 4))
    sns.heatmap(conf_matrix, annot=True, fmt='d', cmap='Blues')
    st.pyplot(plt)

# Scatter Plots of Selected Features
if st.checkbox('Show Scatter Plots'):
    st.subheader('Scatter Plots of Selected Features')
    feature1 = st.selectbox('Select First Feature', data.columns[:-1])
    feature2 = st.selectbox('Select Second Feature', data.columns[:-1])
    
    plt.figure(figsize=(10, 6))
    sns.scatterplot(data=data, x=feature1, y=feature2, hue='Potability', alpha=0.6)
    plt.title(f'Scatter Plot of {feature1} vs {feature2}')
    st.pyplot(plt)

# Pairwise Correlation Plot
if st.checkbox('Show Pairwise Correlation Plot'):
    st.subheader('Pairwise Correlation Plot')
    sns.pairplot(data, hue='Potability')
    st.pyplot(plt)

# Feature Importance (only for linear kernel)
if st.checkbox('Show Feature Importance'):
    st.subheader('Feature Importance (Linear Kernel Only)')
    svm_classifier_linear = SVC(kernel='linear', C=1.0)
    svm_classifier_linear.fit(X_train_scaled, y_train)
    
    importance = svm_classifier_linear.coef_[0]
    feature_importance = pd.DataFrame({'Feature': X.columns, 'Importance': importance})
    feature_importance = feature_importance.sort_values(by='Importance', ascending=False)

    plt.figure(figsize=(10, 6))
    sns.barplot(x='Importance', y='Feature', data=feature_importance)
    st.pyplot(plt)
