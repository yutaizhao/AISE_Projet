# AISE PROJET 

## les fichiers principaux sont organises de la maniere suivante

```bash
├──build
│   ├──bin
│   └──Makefile
├──client.c
├──server.c
├──server_lib.c
├──server_lib.h
├──server.h
└──server.c
```
## Remarques  

### Execution 

En essayant de faire la question avancée (N vers M), nous voulions creer 2 sockets avec 2 adresses Ip differentes dans `server.c`.

Mais en fin nous sommes arrivés à creer 2 sockets avec 2 ports differents. Vu que le temps restant ne nous permet pas d'avancer sur cette question ( En spcecifiant par exemple les addresse Ip et les ports dans `server.c` ) , le programme `server` s'execute ainsi de maniere un peu differente (malheuresement) :

le serveur s'execute de la maniere suivante : 
```bash
./server [port1] [port2]
```

le client s'execute de la maniere suivante : 
```bash
./server [ip] [port1/port2]
```
### 1ere Execution 

Nous avons ajouté une fonctionalite de "Whitelist" dans notre serveur

Lors de la 1ere execution, 2 configs `server_config.txt` et `config.txt` seront crees automatiquement pour resp. serveur et client, et les programmes seront interompus.

Vous avez besoin d'ajouter dans `config.txt` votre identifiant par exemple `votre_nom`, ainsi d'ajouter tous les identifiants des clients voulant se connecter au serveur dans `server_config.txt`.

Les identifiants doivent etre enregistres par lignes, par exemple : 

```bash
Yutai
Chun
APerson
```

Une fois c'est fait, redemarrez le serveur. 

### Commandes supplementaires (Non REDIS)

les donnees qui sont stockes par la commande 
```bash
SAVE
```
peuvent etre re-organisees par 
```bash
SORT
```
peuvent etre extraites par 
```bash
FETCH [AKey]
```

Ces deux dernieres commandes ne sont pas de REDIS, en effet, elles sont justes conçues pour l'implementation de "External Memory Algorithm"

## Commits

Il y a des fichiers non utilisés dans plusieurs commits(recent), notamment ce qui concerne le type "hashtable" et "consistant hashing". Normalement ils n'incluencent pas la compilation, vous vous inquitez pas. 

## References
https://blog.csdn.net/strongwangjiawei/article/details/7786085

https://www.runoob.com/cprogramming/c-function-fseek.html

https://stackoverflow.com/questions/23658023/speed-of-fprintf-vs-fwrite

https://blog.csdn.net/qq_33951180/article/details/72801228

https://stackoverflow.com/questions/23658023/speed-of-fprintf-vs-fwrite

https://morven.life/posts/consistent-hash/

https://github.com/PeterScott/murmur3/blob/master/example.c

https://juejin.cn/post/7047665450632609805

https://zhuanlan.zhihu.com/p/564307507

https://blog.csdn.net/Wmll1234567/article/details/114587876
