// ChatClient.java

import java.io.*;
import java.net.*;

public class ChatClient {
	public static void main(String[] args) throws IOException {

		// check input for correct number of args
		if (args.length != 2) {
			System.err.println("Usage: java ChatClient HOST #PORT");
			System.exit(1);
		}

		// create buffered reader object to read keyboard input
		BufferedReader inputReader = new BufferedReader( new InputStreamReader(System.in));
		String input, handle;

		do
		{
			System.out.print("Enter handle (10 characters or less): ");
			handle = inputReader.readLine();
		}
		while (handle.length() > 10 );
		System.out.println("User handle: " + handle);

		String host = args[0]; // server host 
		int port = Integer.parseInt(args[1]); // server port

		System.out.println("Attempting to connect to port " + port + " on " + host);
		// try to create a socket with Socket(String host, int port)
		// using BufferedReader as wrapper of inputStreamReader  ref: http://stackoverflow.com/questions/1550606/java-sockets-inputstream-read-vs-bufferedreader-read

		// get socket
		Socket socket = new Socket(host, port);
		// int timeout = 10;
		// socket.setSoTimeout(timeout); // allow read to timeout

		// make PrintWriter for writing to socket
		PrintWriter socketWriter = new PrintWriter(socket.getOutputStream(), true);

		// make BufferedReader for inputStream from socket
		BufferedReader socketReader = new BufferedReader (new InputStreamReader(socket.getInputStream()));

		try {
			Boolean quit = false; // to indicate when session should end

			System.out.println("Connected.");
			System.out.print(handle + "> ");

			while(quit == false ){
				String quit_command = "/quit";
				input = inputReader.readLine();	
				if (input.equals(quit_command)){
					// quit command
					System.exit(0);
				}

				// check if socket is closed
				if(socket.isClosed()){
					System.out.println("Connection closed.");
					System.exit(0);
				}
				socketWriter.println( handle + "> " + input );

				String msg = socketReader.readLine();
				if ( msg == null){
					// socket is closed, no read message
					System.out.println("Connection closed.");
					System.exit(0);
				}
				// print to socket
				System.out.println("host> " + msg );
				System.out.print(handle + "> ");
			}
		}
		// catch UnknownHostException
		catch(UnknownHostException e){
			System.err.println("UnknownHostException: " + e.getMessage());
		}
		// catch IOException
		catch(IOException e){
			System.err.println("IOException: " + e.getMessage());
		}
	}
}
