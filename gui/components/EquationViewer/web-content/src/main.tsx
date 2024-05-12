import React from 'react'
import ReactDOM from 'react-dom/client'
import App, {QueryResponsePair} from './App.tsx'
import 'bootstrap/dist/css/bootstrap.min.css';

const history: QueryResponsePair[] = []
let currentEntry = ""

const root = ReactDOM.createRoot(document.getElementById('root')!)

root.render(
  <React.StrictMode>
      <App history={history} currentEntry={currentEntry} />
  </React.StrictMode>,
)

export function addEntryToHistory(query: string, response: string)
{
  history.push({ query, response })
  root.render(
      <React.StrictMode>
        <App history={history} currentEntry={currentEntry} />
      </React.StrictMode>,
  )
}

export function setCurrentEntry(entry: string)
{
  currentEntry = entry
  root.render(
      <React.StrictMode>
        <App history={history} currentEntry={currentEntry} />
      </React.StrictMode>,
  )
}

// declare global {
//     interface Window {
//         addEntryToHistory: (query: string, response: string) => void;
//         setCurrentEntry: (entry: string) => void;
//     }
// }