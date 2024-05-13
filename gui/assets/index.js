function setCurrentInput(input) {
    const currentInputDiv = document.getElementById("currentInput");
    currentInputDiv.innerHTML = "";

    if (input) {
        const currentInputDivContentWrapper = document.createElement("div");
        currentInputDivContentWrapper.setAttribute("class", "text-bg-dark p-2 shadow rounded");
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
    queryDiv.setAttribute("class", "align-self-end text-bg-dark p-2 shadow rounded");
    queryDiv.innerHTML = query;
    queryResponseDiv.appendChild(queryDiv);

    const responseDiv = document.createElement("div");
    responseDiv.setAttribute("class", "align-self-start text-bg-light p-2 shadow rounded border");
    responseDiv.innerHTML = response;
    queryResponseDiv.appendChild(responseDiv);

    stackElem.insertBefore(queryResponseDiv, currentInputElem);
}

window.setCurrentInput = setCurrentInput
window.addToHistory = addToHistory