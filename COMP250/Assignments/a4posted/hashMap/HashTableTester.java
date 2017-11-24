package hashMap;

import java.util.ArrayList;
import java.io.FileReader;
import java.io.BufferedReader;
import java.io.IOException;
/**
 * Class for testing your implementation of the HashTable class.
 */
public class HashTableTester {

	/**
	 * Returns a list of songs to use for testing the hash table.
	 * @return A list of songs to use for testing the hash table
	 */
	private static ArrayList<Song> initSongList() {
		ArrayList<Song> songs = new ArrayList<Song>();
		songs.add(new Song("Le Premier Bonheur du Jour", "Os Mutantes", 1968));
		songs.add(new Song("Stretch Out And Wait", "The Smiths", 1987));
		songs.add(new Song("Scream", "Black Flag", 1984));
		songs.add(new Song("Europe, After the Rain", "Max Richter", 2002));
		songs.add(new Song("Why Are You Looking Grave?", "Mew", 2005));
		songs.add(new Song("Fallen Angel", "King Crimson", 1974));
		return songs;
	}

	public static void main(String[] args) {
		ArrayList<Song> songs = initSongList();
		MyHashTable<String,Song> songTable;
		int numBuckets = 7;
		// Initialize the hash table.   Key will be the song title.

		songTable = new MyHashTable<String,Song>(numBuckets);
		for (Song song: songs) {
			songTable.put(song.getTitle(), song);
		}

		System.out.println("number of songs: " + songTable.size());
		System.out.println("number of buckets in hashtable: " + songTable.getNumBuckets());

		// Try to retrieve a song
		StringBuffer errors = new StringBuffer();
		Song testSong0 = songTable.get("Scream");
		System.out.println(testSong0);
		if (testSong0 == null || !testSong0.getArtist().equals("Black Flag") || testSong0.getYear() != 1984) {
			errors.append("Failed to retrieve song 'Scream'.\n");
		}			

		//  rehashing changes the capacity of the table, but not the number of entries

		songTable.rehash();
		System.out.println("number of songs: " + songTable.size());
		System.out.println("number of buckets in hashtable: " + songTable.getNumBuckets());


		// Try to retrieve a song
		Song testSong1 = songTable.get("Scream");
		System.out.println(testSong1);
		if (testSong1 == null || !testSong1.getArtist().equals("Black Flag") || testSong1.getYear() != 1984) {
			errors.append("Failed to retrieve song 'Scream'.\n");
		}

		// Try to remove a song
		Song removedSong = songTable.remove("Fallen Angel");
		Song retrievedSong = songTable.get("Fallen Angel");
		if (removedSong == null || !removedSong.getArtist().equals("King Crimson")
				|| removedSong.getYear() != 1974 || retrievedSong != null) {
			errors.append("Failed to remove song 'Fallen Angel'.\n");
		}

		//   PUT MORE TESTS HERE.

		// Display the test results
		System.out.println("---------------\nTEST 1 RESULTS:\n---------------\n");
		if (errors.length() == 0) {
			errors.append("All tests passed successfully.");
		}

		System.out.println(errors.toString());
		System.out.println();

	}

}
