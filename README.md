# PROTOCOALE DE COMUNICAŢIE
# Tema 4 Client Web. Comunicaţie cu REST API.

### Stefan Diana Maria - 322CC

## client.c:

In functie de comanda citita, se apeleaza functia corespunzatoare care trimite `cereri http` si interpreteaza raspunsul.

`EXIT`: 
 - Se iese din program.

`REGISTER`: 
 - Se citeste ***numele de utilizator*** si ***parola*** si se apeleaza `reg`.
 - Se completeaza in `data_buffer`, pe cate o linie, ***numele campurilor*** si ***datele*** si se trimite cererea catre server. Se salveaza raspunsul, iar in functie de prima sa linie se afiseaza `mesajul de reusita` / `eroare` si 
se elibereaza memoria.

`LOGIN`: 
 - Se citeste ***numele de utilizator*** si ***parola*** si se apeleaza `login`.
 - Cererea si afisarea mesajului se fac in mod similar, diferenta fiind ca in raspuns exista si un `cookie` care este extras (functia `get_cookie`), prin care se demonstreaza ca utilizatorul este logat.

`ENTER_LIBRARY`: 
 - Se apeleaza `enter_library`.
 - Cererea si afisarea mesajului se fac in mod similar, insa in raspuns exista si un `token` care este extras (functia `get_token`), prin care se demonstreaza ca utilizatorul este conectat la biblioteca.

`GET_BOOKS`: 
 - Se apeleaza `get_books`.
 - Cererea se face adaugand si `tokenul` extras si se afiseaza lista de carti sau raspunsul care anunta utilizatorul ca nu are acces.

`GET_BOOK`: 
 - Se citeste id-ul si se apeleaza `get_book`.
 - Cererea se face adaugand si `tokenul` extras si se afiseaza datele despre cartea ceruta sau raspunsul care anunta utilizatorul fie ca nu are acces, fie ca nu a introdus un id valid.

`ADD_BOOK`: 
 - Se citeste ***titlul***, ***autorul***, ***genul***, ***editorul*** si ***numarul de pagini***.
 - Se verfica daca numarul de pagini este un numar, caz in care se apeleaza `add_book`.
 - Se trimite ***cererea http*** cu `token` si se afiseaza un mesaj care anunta daca actiunea a avut succes sau nu (`acces nepermis` / `format necorespunzator`).

`DELETE_BOOK`: 
 - Se citeste ***id-ul*** si se apeleaza `delete_book`.
 - Se formeaza ***url-ul*** si se trimite cererea de `delete`, dupa care se afiseaza un mesaj care anunta daca actiunea a avut succes sau nu (`acces nepermis` / `id invalid`).

`LOGOUT`: 
 - Se apeleaza `logout`.
 - Se trimite cererea, se elimina `cookie-ul` si `token-ul` salvate si se afiseaza un `mesaj de succes` / `eroare`.

## request.c:

`compute_get_request`:
 - Se adauga ***url-ul***, ***tipul de date***, ***host-ul***, ***token-ul*** (daca exista), ***cookie-ul*** (daca exista).

`compute_post_request`:
 - Se adauga ***url-ul***, ***host-ul***, ***tipul de continut***, ***dimensiunea datelor***, ***token-ul*** (daca exista), ***cookie-ul*** (daca exista) si ***datele***. Stringul care contine datele se formeaza folosidu-se un obiect `json`, alcatuit din 
liniile matricei transmise ca parametru.

`compute_delete_request`:
 - Se adauga ***url-ul***, ***host-ul***, ***token-ul*** (daca exista), ***cookie-ul*** (daca exista).
