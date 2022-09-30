#include "client/Locale.hpp"

#include "utils.hpp"

std::map<std::string, unsigned> Locale::_langs = {
    {"en", 0},
    {"fr", 1},
};

std::array<std::map<std::string, std::vector<std::string>>, 3> Locale::_translations = {
    {{{"Sign In", {"Connexion"}},
      {"Sign Up", {"Inscription"}},
      {"Username:", {"Nom d'utilisateur :"}},
      {"Password:", {"Mot de passe :"}},
      {"Password or username incorrect. Try again:", {"Mot de passe ou nom d'utilisateur invalide. Réessayer :"}},
      {"Confirm Password:", {"Confirmer le mot de passe :"}},
      {"Passwords didn't match. Try again:", {"Les mots de passe ne correpondent pas. Réessayer :"}},
      {"Username already taken. Try again:", {"Nom d'utilisateur déjà pris. Réessayer :"}},
      {"Exit", {"Quitter"}},
      {"Back", {"Retour"}},
      {"Play Game", {"Jouer"}},
      {"Level Editor", {"Éditeur de Niveaux"}},
      {"Create", {"Créer"}},
      {"Choose Level", {"Choisir un Niveau"}},
      {"Created on", {"Créé le"}},
      {"Created by", {"Créé par"}},
      {"rate", {"note"}},
      {"This menu is only available in the GUI version of the game.", {"Ce menu n'est disponible que dans la version GUI du jeu."}},
      {"Profile", {"Profil"}},
      {"Follows", {"Liste de suivi"}},
      {"Follow", {"Suivre"}},
      {"Leaderboard", {"Classement"}},
      {"Best score:", {"Meilleur score :"}},
      {"Experience:", {"Expérience :"}},
      {"Unfollow", {"Arrêter de suivre"}},
      {"🔍 Search", {"🔍 Rechercher"}},
      {"Search Username:", {"Chercher utilisateur :"}},
      {"Username not found. Try again:", {"Cet utilisateur n'existe pas. Réessayer :"}},
      {"You follow", {"Vous suivez"}},
      {"You already follow this user.", {"Vous suivez déjà cet utilisateur."}},
      {"You unfollow", {"Vous arrêtez de suivre"}},
      {"You don't follow this user", {"Vous ne suivez pas cet utilisateur."}},
      {"Solo", {"Solo"}},
      {"Duo", {"Duo"}},
      {"Easy", {"Facile"}},
      {"Medium", {"Intermédiaire"}},
      {"Hard", {"Difficile"}},
      {"Low", {"Bas"}},
      {"High", {"Haut"}},
      {"Enabled", {"Activé"}},
      {"Disabled", {"Désactivé"}},
      {"Game Settings", {"Paramètres"}},
      {"Start Game", {"Lancer partie"}},
      {"Start Campaign", {"Lancer Campagne"}},
      {"Game Mode:", {"Mode de jeu :"}},
      {"Game Mode", {"Mode de jeu"}},
      {"Packs", {"Thème"}},
      {"Player 1", {"Joueur 1"}},
      {"Player 2", {"Joueur 2"}},
      {"Initial Lives:", {"Nombre de vies :"}},
      {"Initial Lives", {"Nombre de vies"}},
      {"Difficulty:", {"Difficulté :"}},
      {"Difficulty", {"Difficulté"}},
      {"Bonus Probability:", {"Probabilité de bonus :"}},
      {"Bonus Probability", {"Probabilité de bonus"}},
      {"Friendly Fire:", {"Tirs alliés :"}},
      {"Friendly Fire", {"Tirs alliés"}},
      {"Quit Game", {"Quitter la partie"}},
      {"Resume", {"Reprendre"}},
      {"characters", {"caractères"}},
      {"Terminal too small please resize", {"Terminal trop petit"}},
      {"LEVEL", {"NIVEAU"}},
      {"Congratulations!", {"Félicitations !"}},
      {"Try again", {"Réessayer"}},
      {"Settings", {"Paramètres"}},
      {"Admin", {"Administration"}},
      {"Set Language", {"Choisir langue"}},
      {"Controls", {"Contrôles"}},
      {"This pack is locked! Enter a key to unlock corresponding pack:", {"Ce thème est verouillé ! Enter une clé pour déverouiller le thème correspondant :"}},
      {"Pack activated", {"Thème activé"}},
      {"Key invalid", {"Clé invalide"}},
      {"Rating of ", {"Note de "}},
      {"... we'll get it off the database soon", {"... nous allons l'enlever de la database"}},
      {"Was the creator under the effects of illegal substances?", {"Mais ils ont pris de la drogue les développeurs ???"}},
      {"Still better than most AAA games", {"Toujours meilleurs que la plupart des triples A"}},
      {"Too much water", {"Trop d'eau"}},
      {"I love when players do my job", {"D'adore quand les joueurs font notre boulot"}},
      {"What are you waiting for?", {"Qu'attends-tu ?"}},
      {"Level info", {"Infos du niveau"}},
      {"Fonts",{"Police d'écriture"}},
      {"Images",{"Images"}},
      {"Audios",{"Musique et bruitages"}},
      {"Everything else",{"Tout le reste"}},
      {"Credits",{"Turbo remerciements"}},
      {"CLI only support default pack", {"La version CLI ne supporte pas d'autre pack"}}},

     // CONGRATULATIONS
     {
         {"Great, you ended a game, now go start your life.", {"Génial tu as fini un jeu, maintenant tu peux commencer ta vie."}},
         {"Were you expecting congratulations? Loser.", {"Tu t'attendais à des applaudissements ? Loser."}},
         {"You may have won, but at what cost?", {"Tu as peut-être gagné, mais à quel prix ?"}},
     },

     // GAME_OVER
     {
         {"Winners train, Losers complain.", {"Les gagnants s'entrainent, les perdants se plaignent."}},
         {"To be a good loser is to learn how to win.", {"Etre un bon perdant, c'est avant tout apprendre à gagner."}},
         {"Nope.", {"C'est si bon d'être mauvais ?"}},
         {"Git gud.", {"Git gud."}},
         {"YOU DIED", {"T'ES MORT"}},
         {"Why do you even keep trying?", {"Pourquoi t'essayes encore, mec ?"}},
         {"Bruh.", {"Bruh."}},
         {"Error: Skills not found", {"Erreur: talent introuvable."}},

     }}};

unsigned Locale::_lang = 0;

unsigned Locale::getLanguageNumber() noexcept {
  return _lang;
}

void Locale::setLanguage(const std::string& languageISO6391) noexcept {
  _lang = _langs.at(languageISO6391);
}

const std::string& Locale::get(const std::string& englishText, std::size_t category) noexcept {
  if (_lang != 0 && _translations[category].find(englishText) != _translations[category].end()) {
    return _translations[category].at(englishText)[_lang - 1];
  }
  return englishText;
}

const std::string& Locale::getRandomSentence(std::size_t category) noexcept {
  std::size_t random = std::size_t(genRandomInt(0, int(_translations[category].size()) - 1));

  std::map<std::string, std::vector<std::string>>::iterator mapIt = _translations[category].begin();
  for (size_t i = 0; i != random; ++i) {
    ++mapIt;
  }

  return mapIt->first;
}

std::string Locale::ratingSentence(int rate) {
  std::string rate_string;
  switch (rate) {
    case 0:
      rate_string = " " + Locale::get("... we'll get it off the database soon");
      break;
    case 1:
      rate_string = " " + Locale::get("Was the creator under the effects of illegal substances?");
      break;
    case 2:
      rate_string = " " + Locale::get("Still better than most AAA games");
      break;
    case 3:
      rate_string = " " + Locale::get("I love when players do my job");
      break;
    case 4:
      rate_string = " " + Locale::get("Too much water");
      break;
    case 5:
      rate_string = " " + Locale::get("What are you waiting for?");
      break;
  }
  return rate_string;
}
