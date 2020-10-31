Project-part1

Panagiotis Nikolopoulos AM:1115201400128 email: sdi1400128@di.uoa.gr
Politimi Marioli AM:1115201400098 email: sdi1400098@di.uoa.gr

Εντολή μετταγλώτισης: make

Αρχεία:
main_lsh.cpp
Δέχεται τα ορίσματα, τα ελέγχει, δημιουργεί τις απαραίτητες δομές, υπολογίζει το W (είτε το θέτει) και κατά σειρά υπολογίζει για κάθε input data, τον true καζι τον LSH nearest neighbor, τον Νιοστο και σε ακτίνα R αντίστοιχα (για true και LSH) (Συγκεκριμένα offset, distance και time καθώς και τον συνολικό χρόνο γι αυτά (και true και LSH))

main_cube.cpp
Ακριβώς την ίδια διαδικασία, με τη μεθοδολογία του hypercube αντί για την LSH.

helpers.cpp & .h
Υλοποίηση βοηθητικών συναρτήσεων και συγκεκριμένα my_random (random double αριθμός), modulo, exp_modulo και hammilton distance.

ImageData.cpp & h
Υλοποίηση της βασικής δομής των διανυσμάτων εικόνων που χρησιμοποιούμε (της κλάσης που χρειάζεται και των συναρτήσεων print και manhattan που αφορόυν εκτυπωση των bytes του διανύσματος και υπολογισμό manhattan distance αντίστοιχα.

ImageHashTable.cpp & h
Υλοποίηση της δομής των hastables για την αποθήκευση των επεξεργασμένων διανυσμάτων αντίστοιχα για LSH ή hypercube (ανάλογα με τον selector) το καθένα σύμφωνα με τη μεθοδολογία του (αντίστοιχες ολισθήσεις κτλ)

RandomVector.cpp & h
Υλοποίηση ενός vector ώστε να αξιοποιηθεί στα hashtables (κλάση και συνάρτηση fill που επί της ουσίας κάνει pushback)

ImageDataArray.cpp & h


Clustering.cpp & h


ResultNN.h
Η δομή για την επιστροφή αποτελεσμάτων nearest neighbor.Περιλαμβάνει imagedata, τη θέση offset(int), την απόσταση distance (double) και τον χρόνο t 

comparator.h
Δέχεται 2 structs τύπου ResultNN ki eπιστρέφει τη σύγκριση των 2 μεταξύ τους distances

