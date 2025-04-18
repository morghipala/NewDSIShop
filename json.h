#ifndef JSON_H
#define JSON_H

typedef struct {
    char *id;
    char *title;
    char *description;
    char *file_url;
} Game;

// parse lista di giochi (JSON array of objects with "id","title")
Game *parse_game_list(const char *json, int *outCount);
// parse dettagli gioco (JSON object with id,title,description,file_url)
Game parse_game_details(const char *json);
// libera array restituito da parse_game_list
void free_game_list(Game *games, int count);

#endif // JSON_H
