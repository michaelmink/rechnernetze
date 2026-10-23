package bellmanford.com.sim;

import org.springframework.boot.SpringApplication;
import org.springframework.boot.autoconfigure.SpringBootApplication;

import java.util.ArrayList;
import java.util.List;


@SpringBootApplication
public class SimApplication {

	public static void main(String[] args) {
		SpringApplication.run(SimApplication.class, args);
		System.out.println("Hallo.");

		// current estimated distances
		// init at t=0
		List<Integer> est_dist = new ArrayList<>(List.of(Integer.MAX_VALUE, Integer.MAX_VALUE, 4, Integer.MAX_VALUE, 2));

		State my_state = new State();
		my_state.init_graph();

		System.out.println("Nodes: " + my_state.getNodes());
		System.out.println("Edges: " + my_state.getEdges());

		// first timestep
		int timestep = 1;

		// example for node u, later loop over all
		bellmanford(est_dist, "u", my_state.getEdgesforNode("u"));
	}

	private static void bellmanford(List<Integer> est_dist, String node, List<State.Edge> edges) {
		System.out.println("Perform Bellman-Ford on Node " + node);
		System.out.println("edges for node " + node + ": " + edges);
		System.out.println("node idx for " + node + " is " + NodeMapping.GetIdxforNode(node));
		System.out.println("Distance from w->z: " + est_dist.get(NodeMapping.GetIdxforNode("w")));

	}

}
