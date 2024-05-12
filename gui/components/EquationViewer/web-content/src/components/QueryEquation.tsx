import {useEffect, useRef} from "react";

interface QueryEquationProps {
    scrollIntoView?: boolean;
    mathml: string;
}

function QueryEquation({ scrollIntoView, mathml }: QueryEquationProps) {

    const divRef = useRef<HTMLDivElement>(null);

    useEffect(() => {
        if (scrollIntoView && divRef.current) {
            divRef.current.scrollIntoView();
        }
    })

    return (
        <div ref={divRef} className="align-self-end text-bg-dark p-2 rounded shadow" dangerouslySetInnerHTML={{__html: mathml}}></div>
    )
}

export default QueryEquation;