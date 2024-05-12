import {Container, Stack} from "react-bootstrap";
import QueryEquation from "./components/QueryEquation.tsx";
import ResponseEquation from "./components/ResponseEquation.tsx";

export interface QueryResponsePair {
    query: string;
    response: string;
}

interface AppProps {
    history: QueryResponsePair[];
    currentEntry: string
}

function App({history, currentEntry}: AppProps) {
    return (
        <Container>
            <Stack gap={2}>
                {history.map((entry, index) => (
                    <Stack gap={2} key={index}>
                        <QueryEquation mathml={entry.query}/>
                        <ResponseEquation mathml={entry.response}/>
                    </Stack>
                ))}
                {currentEntry && <QueryEquation scrollIntoView mathml={currentEntry}/>}
            </Stack>
        </Container>
    )
}

export default App