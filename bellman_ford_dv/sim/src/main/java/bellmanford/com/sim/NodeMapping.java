package bellmanford.com.sim;

import java.util.ArrayList;
import java.util.List;

public enum NodeMapping {
	U(0, "u"),
    V(1, "v"),
    W(2, "w"),
    X(3, "x"),
    Y(4, "y");

	private final int idx;

	NodeMapping(int idx, String node) {
		this.idx = idx;
	};
	
	public static int GetIdxforNode(String node){return valueOf(node.toUpperCase()).idx;}
}
