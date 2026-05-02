# LAB 22 — Android JNI (Java Native Interface)

Application Android de démonstration de l'intégration de code natif C++ via JNI (Java Native Interface) et le NDK Android.

---

## Aperçu

Ce laboratoire montre comment appeler du code C++ depuis Java dans une application Android en utilisant :

- **JNI** — Java Native Interface, la passerelle Java ↔ C++
- **NDK** — Native Development Kit, les outils de compilation native Android
- **CMake** — Le système de build pour la bibliothèque native

### Fonctionnalités démontrées

| Fonction | Description | Type retourné |
|---|---|---|
| `helloFromJNI()` | Retourne une chaîne depuis le C++ | `String` |
| `factorial(int n)` | Calcul de factoriel avec gestion d'overflow | `int` |
| `reverseString(String s)` | Inversion d'une chaîne Java → C++ → Java | `String` |
| `sumArray(int[] values)` | Somme d'un tableau envoyé au natif | `int` |

---

## Architecture

```
Java / MainActivity
        │
        │  appelle méthode native
        ▼
System.loadLibrary("native-lib")
        │
        │  charge
        ▼
libnative-lib.so  (compilée par CMake + NDK)
        │
        │  JNI transmet l'appel
        ▼
native-lib.cpp  (code C++)
        │
        │  retourne le résultat converti
        ▼
Java / affichage dans les TextView
```

---

## Structure du projet

```
JNIDemo/
├── app/
│   ├── src/
│   │   └── main/
│   │       ├── cpp/
│   │       │   ├── CMakeLists.txt       ← configuration build natif
│   │       │   └── native-lib.cpp       ← code C++ avec les 4 fonctions JNI
│   │       ├── java/com/example/jnidemo/
│   │       │   └── MainActivity.java    ← déclaration native + appels
│   │       └── res/layout/
│   │           └── activity_main.xml    ← interface 4 TextView
│   └── build.gradle.kts                 ← config Gradle avec externalNativeBuild
└── README.md
```

---

## Prérequis

Avant de cloner et compiler, vérifier que les éléments suivants sont installés via **Tools → SDK Manager → SDK Tools** :

- [x] Android Studio (version récente)
- [x] Android SDK
- [x] NDK (Side by side)
- [x] CMake
- [x] LLDB

---

## Installation

### 1. Cloner le dépôt

```bash
git clone https://github.com/votre-utilisateur/JNIDemo.git
cd JNIDemo
```

### 2. Ouvrir dans Android Studio

```
File → Open → sélectionner le dossier JNIDemo
```

### 3. Synchroniser Gradle

```
File → Sync Project with Gradle Files
```

### 4. Lancer l'application

```
Run → Run 'app'  (émulateur API 33+ ou téléphone réel)
```

---

## Résultats attendus

À l'écran après lancement :

```
Hello from C++ via JNI !
Factoriel de 10 = 3628800
Texte inverse : !lufrewop si INJ
Somme du tableau = 150
```

Dans Logcat (filtrer par tag `JNI_DEMO`) :

```
Appel de helloFromJNI depuis le natif
Factoriel de 10 calcule en natif = 3628800
String inversee = !lufrewop si INJ
Somme du tableau = 150
```

---

## Fichiers clés expliqués

### `CMakeLists.txt`

Décrit comment compiler la bibliothèque native :

```cmake
cmake_minimum_required(VERSION 3.22.1)
project("jnidemo")

add_library(native-lib SHARED native-lib.cpp)

find_library(log-lib log)

target_link_libraries(native-lib ${log-lib})
```

- `add_library` → crée `libnative-lib.so`
- `find_library(log-lib log)` → accès à `__android_log_print` pour Logcat
- `target_link_libraries` → lie la bibliothèque `log` au natif

---

### `native-lib.cpp`

Chaque fonction suit la convention de nommage JNI stricte :

```
Java_<package>_<Classe>_<méthode>
     ↓
Java_com_example_jnidemo_MainActivity_helloFromJNI
```

> ⚠️ Si le package ou le nom de classe change, la signature C++ doit être mise à jour, sinon l'application lèvera une `UnsatisfiedLinkError`.

Points importants :
- `extern "C"` — empêche le name mangling C++
- `GetStringUTFChars` / `ReleaseStringUTFChars` — conversion String Java ↔ C (toujours libérer après usage)
- `GetIntArrayElements` / `ReleaseIntArrayElements` — accès aux tableaux Java depuis le natif

---

### `MainActivity.java`

```java
// Déclaration des méthodes natives
public native String helloFromJNI();
public native int factorial(int n);
public native String reverseString(String s);
public native int sumArray(int[] values);

// Chargement de la bibliothèque (obligatoire)
static {
    System.loadLibrary("native-lib");
}
```

> Le nom passé à `loadLibrary()` doit être `native-lib`, pas `libnative-lib.so`.

---

## Gestion des erreurs

### Codes de retour `factorial()`

| Code | Signification |
|---|---|
| `>= 0` | Résultat valide |
| `-1` | Valeur négative passée en entrée |
| `-2` | Dépassement de `INT_MAX` (overflow) |

### Codes de retour `sumArray()`

| Code | Signification |
|---|---|
| `>= 0` | Somme valide |
| `-1` | Tableau `null` |
| `-2` | Impossible d'accéder aux éléments |
| `-3` | Overflow sur la somme |

---

## Cas de test

```java
factorial(10)          // → 3628800  (normal)
factorial(-5)          // → -1       (valeur négative)
factorial(20)          // → -2       (overflow int)
reverseString("")      // → ""       (chaîne vide)
sumArray(new int[]{})  // → 0        (tableau vide)
```

---

## Erreurs fréquentes

### `UnsatisfiedLinkError`

Causes possibles :
- Nom incorrect dans `System.loadLibrary("...")`
- Signature JNI ne correspond pas au package/classe Java
- Fichier `.so` non généré (vérifier CMakeLists.txt et Gradle)

### Crash sur manipulation de String

Cause : oubli de `ReleaseStringUTFChars` après `GetStringUTFChars`.  
Toujours libérer les ressources JNI après usage pour éviter les fuites mémoire.

### Compilation C++ échoue

Vérifier que les includes suivants sont présents dans `native-lib.cpp` :

```cpp
#include <algorithm>   // pour std::reverse
#include <climits>     // pour INT_MAX
#include <android/log.h> // pour __android_log_print
```

---

## Quand utiliser JNI ?

JNI n'est pas adapté à tous les cas. Il est pertinent pour :

- **Calcul intensif** : traitement d'image, chiffrement, moteur physique
- **Réutilisation de bibliothèques C/C++** : OpenCV, moteurs audio/vidéo
- **Protection partielle de logique** : le code natif est plus difficile à reverse
- **Accès bas niveau** : API natives Android proches du système

---

## Bonnes pratiques appliquées

- Logs natifs via `LOGI` / `LOGE` avec tag `JNI_DEMO`
- Gestion systématique des erreurs avec codes de retour
- Libération des ressources JNI après chaque `Get...`
- Vérification des pointeurs nuls avant usage
- `extern "C"` sur chaque fonction exportée

---

## Configuration

- **Minimum SDK** : API 33
- **Target SDK** : API 36
- **CMake** : 3.22.1+
- **Java** : VERSION_11
- **Build system** : Gradle (Kotlin DSL) + CMake
