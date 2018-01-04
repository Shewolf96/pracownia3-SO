# Pracownia 3 - SO

### Raport 03.01.2018

Dotychczasowa realizacja wymagała dość dużo czytania, przede wszystkim żeby ostatecznie zdecydować się na (chyba najprostszy) sposób zasymulowania wielowątkowości, poprzez oddzielenie każdej instrukcji wywołaniem "schedulera", zamiast próby przełączania wątków co regularny odstęp czasu.


Co jeszcze trzeba zrobić:

- być może dodać funkcję join_thread(thread *t);
- dopisać pozostałe funkcje zawarte w dokumentacji
- przetestować na początku dla bardzo prostych programów wielowątkowych
- znaleźć przykład, dla którego może zdarzyć się deadlock, a potem wymyślić i zaimplementować rozwiązanie takiej sytuacji
