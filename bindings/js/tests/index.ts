import test from "node:test";
import assert from "node:assert/strict";
import loadOasis from "../oasis-node"

test("Addition of Real Numbers", async () => {
    const Oasis = await loadOasis();
    const add = Oasis.FromInFix("2 + 2");
    const addResult = Oasis.Simplify(add);
    const addResultStr = Oasis.ToMathMLString(addResult);
    assert.equal(addResultStr, "<mn>4</mn>\n");
});

test("Parsing and Serializing of Integrals", async () => {
    const Oasis = await loadOasis();
    const integral = Oasis.FromInFix("in ( 1 , x )");
    const integralML = Oasis.ToMathMLString(integral);
    assert.equal(integralML, `<mrow>
    <mo>∫</mo>
    <mn>1</mn>
    <mrow>
        <mo>d</mo>
        <mi>x</mi>
    </mrow>
</mrow>
`);
});