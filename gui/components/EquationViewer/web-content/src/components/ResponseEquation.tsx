function ResponseEquation({ mathml }: { mathml: string }) {
    return (
        <div className="align-self-start text-light-dark p-2 rounded border shadow" dangerouslySetInnerHTML={{__html: mathml}}></div>
    )
}

export default ResponseEquation;