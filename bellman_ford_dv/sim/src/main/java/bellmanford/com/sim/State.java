package bellmanford.com.sim;

import java.util.ArrayList;
import java.util.List;

public class State {

    public record Edge(String from, String to, int cost) {}

    private final List<String> nodes = new ArrayList<>();
    private final List<Edge> edges = new ArrayList<>();

    public void init_graph() {
        System.out.println("Init Graph: ");

        nodes.add("u");
        nodes.add("v");
        nodes.add("x");
        nodes.add("w");
        nodes.add("y");
        nodes.add("z");

        edges.add(new Edge("u", "v", 2));
        edges.add(new Edge("u", "w", 5));
        edges.add(new Edge("u", "x", 1));
        edges.add(new Edge("x", "u", 1));
        edges.add(new Edge("x", "v", 2));
        edges.add(new Edge("x", "w", 3));
        edges.add(new Edge("x", "y", 1));
        edges.add(new Edge("v", "u", 2));
        edges.add(new Edge("v", "x", 2));
        edges.add(new Edge("v", "w", 3));
 
    }

    public List<String> getNodes() { 
        return nodes; 
    }
    
    public List<Edge> getEdges() { 
        return edges; 
    }

    public List<Edge> getEdgesforNode(String node_wanted) {
        List<Edge> edges_filter = new ArrayList<>();

        for (Edge edge : edges) {
            if (edge.from().equals(node_wanted)) {
                edges_filter.add(edge);
            }
        }

        return edges_filter;
    }
}
