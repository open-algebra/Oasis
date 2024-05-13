function setCurrentInput(input) {
    const currentInputDiv = document.getElementById("currentInput");
    currentInputDiv.innerHTML = "";

    if (input) {
        const currentInputDivContentWrapper = document.createElement("div");
        currentInputDivContentWrapper.setAttribute("class", "bg-primary-subtle text-primary-emphasis p-2 shadow rounded-4 border");
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
    queryDiv.setAttribute("class", "align-self-end bg-primary-subtle text-primary-emphasis p-2 shadow rounded-4 border");
    queryDiv.innerHTML = query;
    queryResponseDiv.appendChild(queryDiv);

    const responseDivContainer = document.createElement("div");
    responseDivContainer.setAttribute("class", "d-flex flex-row gap-2");

    const foxSvgContainer = document.createElement("div")
    foxSvgContainer.setAttribute("class", "bg-white align-self-end p-1 rounded")

    const foxSvg = document.createElement("img");
    foxSvg.setAttribute("src", "memory:Fox.svg");
    foxSvg.setAttribute("style", "height: 2em; width: 2em;")
    foxSvgContainer.appendChild(foxSvg);
    responseDivContainer.appendChild(foxSvgContainer);

    const responseDiv = document.createElement("div");
    responseDiv.setAttribute("class", "align-self-start bg-secondary-subtle p-2 shadow rounded-4 border");
    responseDiv.innerHTML = response;
    responseDivContainer.appendChild(responseDiv);

    queryResponseDiv.appendChild(responseDivContainer)
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