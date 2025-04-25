import json
from graphviz import Digraph  # type: ignore

def draw_min_dfa_from_json(json_file: str, output_file: str = "min_dfa_visual"):
    with open(json_file, "r") as f:
        data = json.load(f)

    dot = Digraph(comment="Minimized DFA", format="png")
    dot.attr(rankdir="LR")

    accept_states = set(data.get("accept", []))
    dead_states = set(data.get("dead", []))

    for state in data["states"]:
        shape = "doublecircle" if state in accept_states else "circle"
        style = "dashed" if state in dead_states else "solid"
        color = "gray" if state in dead_states else "black"
        dot.node(str(state), shape=shape, style=style, color=color)

    dot.node("start", shape="point")
    dot.edge("start", str(data["start"]), label="start")

    for t in data["transitions"]:
        dot.edge(str(t["from"]), str(t["to"]), label=t["symbol"])

    dot.render(output_file, view=True)
    print(f"[OK] Rendered Minimized DFA to: {output_file}.png")

if __name__ == "__main__":
    draw_min_dfa_from_json("output/min_dfa.json", output_file="visualize/min_dfa_visual")
