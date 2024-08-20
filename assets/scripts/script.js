// Alert Button Event Listener
document.getElementById('alertButton').addEventListener('click', function() {
    alert('Hello, Pasindu Fernando! Welcome to your Web Server Demo.');
    displayDynamicContent();
});

// Function to Display Dynamic Content
function displayDynamicContent() {
    const dynamicContent = document.getElementById('dynamicContent');
    const date = new Date();
    dynamicContent.innerHTML = `<p>Current Date and Time: ${date.toLocaleString()}</p>`;
}
