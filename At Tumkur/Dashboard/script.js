document.addEventListener('DOMContentLoaded', () => {
    // Function to show a specific section
    const showSection = (sectionId) => {
        const sections = document.querySelectorAll('main > section');
        sections.forEach(section => {
            section.classList.add('hidden'); // Hide all sections
        });
        document.getElementById(sectionId).classList.remove('hidden'); // Show the selected section
    };

    // Sample alerts
    const alerts = [
        'Alert: Water quality is critical!',
        'Alert: New data points received.',
        'Alert: Maintenance scheduled for next week.'
    ];

    const alertList = document.getElementById('alertList');
    alerts.forEach(alert => {
        const div = document.createElement('div');
        div.textContent = alert;
        alertList.appendChild(div);
    });

    // Default to show visualization
    showSection('visualization');
});
