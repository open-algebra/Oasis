function setCurrentInput(input) {
    const currentInputDiv = document.getElementById("currentInput");
    currentInputDiv.innerHTML = "";

    if (input) {
        const currentInputDivContentWrapper = document.createElement("div");
        currentInputDivContentWrapper.setAttribute("class", "bg-primary-subtle text-primary-emphasis p-2 shadow rounded-4");
        currentInputDivContentWrapper.innerHTML = input;

        currentInputDiv.appendChild(currentInputDivContentWrapper)
    }

    window.scrollTo(0, document.body.scrollHeight);
}

function addToHistory(query, response) {
    const stackElem = document.getElementById("stack");
    const currentInputElem = document.getElementById("currentInput");

    const queryResponseDiv = document.createElement("div");
    queryResponseDiv.setAttribute("class", "d-flex flex-column gap-1");

    const queryDiv = document.createElement("div");
    queryDiv.setAttribute("class", "align-self-end bg-primary-subtle text-primary-emphasis p-2 shadow rounded-4");
    queryDiv.innerHTML = query;
    queryResponseDiv.appendChild(queryDiv);

    const responseDiv = document.createElement("div");
    responseDiv.setAttribute("class", "align-self-start bg-secondary-subtle p-2 shadow rounded-4");
    responseDiv.innerHTML = response;
    queryResponseDiv.appendChild(responseDiv);

    stackElem.insertBefore(queryResponseDiv, currentInputElem);
}

function setLightTheme()
{
    document.documentElement.setAttribute("data-bs-theme", "light");
}

function setDarkTheme()
{
    document.documentElement.setAttribute("data-bs-theme", "dark");
}

window.setCurrentInput = setCurrentInput
window.addToHistory = addToHistory
window.setLightTheme = setLightTheme
window.setDarkTheme = setDarkTheme