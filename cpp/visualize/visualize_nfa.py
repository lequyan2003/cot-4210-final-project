import json
from graphviz import Digraph  # type: ignore

def draw_nfa_from_json(json_file: str, output_file: str = "nfa_visual"):
    with open(json_file, "r") as f:
        data = json.load(f)

    dot = Digraph(comment="NFA", format="png")
    dot.attr(rankdir="LR")

    for state in data["states"]:
        shape = "doublecircle" if state == data["accept"] else "circle"
        dot.node(str(state), shape=shape)

    dot.node("start", shape="point")
    dot.edge("start", str(data["start"]), label="start")

    for t in data["transitions"]:
        label = t["symbol"]
        style = "dashed" if label == "eps" else "solid"
        color = "blue" if label == "eps" else "black"
        dot.edge(str(t["from"]), str(t["to"]), label=label, style=style, color=color)

    dot.render(output_file, view=True)
    print(f"[OK] Rendered NFA to: {output_file}.png")

if __name__ == "__main__":
    draw_nfa_from_json("output/nfa.json", output_file="visualize/nfa_visual")
