package concatenateFiles;
import java.io.*;

public class FileConcatenator {
	public static void main(String[] args) throws IOException {
		/*Read the output files from each folder and compare them. Find the best one, and write it
		 * into outputs/testResults.out
		 */

		String[] bestResultsArray = new String[622];
		for(int i = 1; i < 622; i++) {
			File[] files = new File[4];
			files[0] = new File("outputs" + "1/" + i + ".out");
			files[1] = new File("outputs" + "2/" + i + ".out");
			files[2] = new File("outputs" + "3/" + i + ".out");
			
			BufferedReader output;
			int bestResultsCount = -1;
			for(int j = 0; j < 3; j++) {
				output = new BufferedReader(new FileReader(files[j]));	
				String edges = output.readLine();
				int count = Integer.parseInt(edges);
				if (count > bestResultsCount) {
					bestResultsCount = count;
					bestResultsArray[i] = output.readLine();
				}
			}
		}
		
		PrintWriter out = new PrintWriter("outputs/testResults.out");
		//Output the Strings into the out file
		for(int i = 1; i < 622; i++) {
			out.println(bestResultsArray[i]);
		}
		out.close();
	}
}
