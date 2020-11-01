Project-part1

Panagiotis Nikolopoulos AM:1115201400128 email: sdi1400128@di.uoa.gr

Politimi Marioli AM:1115201400098 email: sdi1400098@di.uoa.gr

Εντολή μετταγλώτισης: make ή make all 

Εντολές χρήσης του προγράμματος: 
*LSH:        "${OUTPUT_PATH}" -selector 0 -d train-images.idx3-ubyte -q t10k-images.idx3-ubyte -k 3 -L 4 -o output.txt -N 5 -R 5000
*Hypercube:  "${OUTPUT_PATH}" -selector 0 -d train-images.idx3-ubyte -q t10k-images.idx3-ubyte -k 3 -L 4 -o output.txt -N 5 -R 5000
*Clustering: -Classic: ${OUTPUT_PATH}" -i t10k-images.idx3-ubyte -o output.txt -c clustering.conf -complete -m Classic
	            -Lsh: ${OUTPUT_PATH}" -i t10k-images.idx3-ubyte -o output.txt -c clustering.conf -complete -m LSH
	            -Hypercube: ${OUTPUT_PATH}" -i t10k-images.idx3-ubyte -o output.txt -c clustering.conf -complete -m Hypercube

Αρχεία:

* main_lsh.cpp:
Δέχεται τα ορίσματα, τα ελέγχει, δημιουργεί τις απαραίτητες δομές, υπολογίζει το W (είτε το θέτει) και κατά σειρά υπολογίζει για κάθε input data, τον true και τον LSH nearest neighbor, τον Νιοστο nearest neighbor και τους nearest neighbors σε ακτίνα R αντίστοιχα (για true και LSH) (Συγκεκριμένα offset, distance και time καθώς και τον συνολικό χρόνο γι αυτά (και true και LSH))

* main_cube.cpp:
Ακριβώς την ίδια διαδικασία, με τη μεθοδολογία του hypercube αντί για την LSH.

* main_clustering.cpp:
Δέχεται τα ορίσματα, τα ελέγχει, δημιουργεί τις απαραίτητες δομές,δέχεται μέσω του ορίσματος selector εάν θα τρέξει η διαδικασία Lsh, Hypercube ή Classic (0,1,2) και στη συνέχεια κάνει initialize τα centroids. Κάνει assign τα input data στα centroid σύμφωνα με την επιλεγμένη μεθοδολογία, κάνει update τα centroids στα modified δεδομένα (δηλαδή μετά τα assign), κάνει visualize για κάθε περίπτωση σε επανάληψη 15 φορές. Στο τέλος τρέχει Silhouette για τα τελικά modified δεδομένα αλλά και visualize συνολικά. Παράλληλα χρονομετρεί την όλη διαδικασία.

Όλες οι main κάνουν σωστή διαχείριση μνήμης με τα απαραίτητα new και delete στις δομές που αξιοποιούν. 

 * helpers.cpp & .h:
Υλοποίηση βοηθητικών συναρτήσεων και συγκεκριμένα my_random (random double αριθμός), modulo, exp_modulo και hammilton distance.

 * ImageData.cpp & h:
Υλοποίηση της βασικής δομής των διανυσμάτων εικόνων που χρησιμοποιούμε (της κλάσης που χρειάζεται και των συναρτήσεων print και manhattan που αφορόυν εκτυπωση των bytes του διανύσματος και υπολογισμό manhattan distance αντίστοιχα.

 * ImageHashTable.cpp & h:
Υλοποίηση της δομής των hastables για την αποθήκευση των επεξεργασμένων διανυσμάτων αντίστοιχα για LSH ή hypercube (ανάλογα με τον selector) το καθένα σύμφωνα με τη μεθοδολογία του (αντίστοιχες ολισθήσεις κτλ)
 
* RandomVector.cpp & h:
Υλοποίηση ενός vector ώστε να αξιοποιηθεί στα hashtables (κλάση και συνάρτηση fill που επί της ουσίας κάνει pushback)

 * ImageDataArray.cpp & h:
Οι πίνακες που περιλαμβάνουν ImageData. Δημιουργία της δομής και υλοποίηση όλων των πράξεων που αξιοποιούνται στα main αρχεία. Συγκεκριμένα:
Άνοιγμα των αρχείων και κατάλληλη αποθήκευση των δεδομένων, υλοποίηση nearest neighbor (και οι 3 περιπτώσεις), υλοποίηση της calculateW και της distance που αξιοποιείται στο clustering για την απόσταση 2 διανυσμάτων εικόνων.

 * Clustering.cpp & h:
Υλοποίηση όλων των βασικών συναρτήσεων για το clustering.
Initialization: αρχική επιλογή centroids. Του πρώτου εντελώς τυχαία και των υπολοίπων με βάση το τυχαίο r αφού υπολογίσουμε τις αποστάσεις όλων των inputdata από το centroid.
AssignmentClassic: Κάνει assign τα Input data σε centroids με βάση τη manhattan distance τους από το κάθε centroid (το centroid με τη μικρότερη manh. dist. από κάθε input data ανατείθεται σε αυτό)
AssignmentLSH: Κάνει assign τα Input data σε centroids με βάση τη μεθοδολογία LSH και ακτίνα R, η οποία διπλασιάζεται κάθε φορά σε κάθε επανάληψη. Τα Input data, γίνονται assign σε κάθε centroid την πρώτη φορά που ανακαλύπτονται, δεν συμβαίνει τίποτα εάν έχουν ήδη γίνει assign στο centroid που τα βρίσκει στη συγκεκριμένη επανάληψη, ή εάν έχει ανακαλυφθεί από άλλο centroid σε προηγούμενη επανάληψη πάλι δε συμβαίνει τίποτα μιας και από το άλλο centroid θα απέχουν σίγουρα μικρότερη απόσταση (αφού βρέθηκε με μικρότερη ακτίνα R).
AssignmentHypercube: Ακριβώς η ίδια διαδικασία με τη μεθοδολογία και το hashing του Hypercube αυτή τη φορά.
Update: Κάνει update τα centroids κάθε cluster σε καλύτερα, σύμφωνα με τη μεθοδολογία του median σε κάθε cluster, κάνει έλεγχο για το μέγεθος των διαστάσεων κι επιστρέφει τα νέα centroids.
Silhouette: Υλοποίηση του αλγορίθμου Silhouette για την αξιολόγηση του clustering που έχουμε υλοποιήση. Υπολογισμός double τιμής -1 έως 1. -1 ή 0 "κακή" συσταδοποίηση - τιμή κοντά στο 1 "καλή" συσταδοποίηση.
Visualize: Τυπώνει για κάθε cluster, αν του έχει γίνει κάποιο assignment και αν ναι, το μέγεθος του. 'Αν το clustering έχει ολοκληρωθεί και υπάρχουν assignments τυπώνει επίσης για κάθε cluster το μέγεθος του και για κάθε ένα assignment που του έχει γίνει το id του.


* ResultNN.h:*
Η δομή για την επιστροφή αποτελεσμάτων nearest neighbor.Περιλαμβάνει imagedata, τη θέση offset(int), την απόσταση distance (double) και τον χρόνο t 

 * comparator.h:
Δέχεται 2 structs τύπου ResultNN κι eπιστρέφει τη σύγκριση των 2 μεταξύ τους distances

