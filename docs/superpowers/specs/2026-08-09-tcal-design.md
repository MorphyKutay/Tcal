# tcal — Terminal Takvim Uygulaması (Tasarım)

## Amaç

C ve ncurses ile, nvim benzeri modal etkileşime sahip, terminalde çalışan
aylık takvim/görev uygulaması. Kullanıcı hjkl ile günler arasında gezinir,
`:` ile komut moduna girer, `:e` ile seçili güne görev ekler.

## Görsel tasarım

Aylık grid, [taskim](https://github.com/RohanAdwankar/taskim) projesindeki
gibi paylaşılan kenarlıklı bir tablo olarak çizilir. Her gün kendi hücresinde:
üstte gün numarası, altında (sığdığı kadar, en fazla 2 satır) görev başlıkları.
Fazlası varsa son satırda `+N daha` gösterilir. Seçili gün ters renkle (A_REVERSE)
vurgulanır. Ayrı bir görev paneli yoktur — her şey grid içinde gösterilir.

```
┌──────────────────────── Ağustos 2026 ────────────────────────┐
│   Pt      Sa      Ça      Pe      Cu      Ct      Pz          │
├────────┬────────┬────────┬────────┬────────┬────────┬────────┤
│        │        │        │        │        │        │      1 │
│        │        │        │        │        │        │        │
├────────┼────────┼────────┼────────┼────────┼────────┼────────┤
│      2 │      3 │      4 │      5 │      6 │      7 │      8 │
│        │        │        │        │        │        │        │
├────────┼────────┼────────┼────────┼────────┼────────┼────────┤
│      9░│     10 │     11 │     12 │     13 │     14 │     15 │
│* Rapor░│        │        │        │        │        │        │
│* Diş  ░│        │        │        │        │        │        │
├────────┼────────┼────────┼────────┼────────┼────────┼────────┤
└─────────────────────────────────────────────────────────────┘
:e_
```

Her hafta satırı sabit yükseklikte: 1 gün-numarası satırı + 2 görev satırı
(toplam 3 iç satır + üst/alt kenarlık). Görev metni hücre genişliğine göre
kesilir (`* Rapor teslim` → `* Rapor ` gibi).

## Seçim modeli (grid içi, ayrı panel yok)

`taskim`'deki gibi seçim iki türden biri olabilir:

- **Gün seçimi**: hjkl ile günler arası gezinir (hücre vurgulanır).
- **Görev seçimi**: seçili günün içinde bir görev satırı vurgulanır (farklı
  stil, örn. altı çizili/bold). `j` ile gün seçiminden o günün ilk görevine,
  sonra sıradaki görevlere; son görevden sonra `j` bir sonraki haftaya
  (gün seçimine) geçer. `k` tersi yönde. `h`/`l` her zaman gün bazında
  yatay hareket eder (görev seçimindeyken önce o günün gün-seçimine döner).

Bu davranış `move_up`/`move_down` mantığıyla birebir aynı: görev varsa önce
görevler arasında gezin, yoksa hafta atla.

## Klavye / komut haritası

Normal mod:
- `h j k l` / ok tuşları — yukarıdaki seçim modeline göre gezinme
- `p` / `n` — önceki/sonraki ay
- `:` — komut moduna gir, en alt satırda `:` istemi belirir

Komut modu (`:` sonrası yazılıp Enter ile onaylanır, Esc ile iptal):
- `e` → `Görev: ` giriş kutusu açılır; girilen metin **seçili güne** (gün
  seçiliyse o gün, görev seçiliyse görevin ait olduğu gün) yeni görev
  olarak eklenir ve dosyaya yazılır.
- `d` → seçim bir görev üzerindeyse o görevi siler. Gün seçiliyse etkisi
  yoktur (silinecek görev yok).
- `q` → kaydet ve çık.

## Veri modeli ve kalıcılık

```c
typedef struct {
    char date[11];   // "YYYY-MM-DD"
    char text[256];
} Task;
```

Dinamik dizi (`Task *tasks; size_t count, capacity`) bellekte tutulur.
Dosya: `~/.local/share/tcal/tasks.txt`, satır formatı `YYYY-MM-DD|görev metni`.
Program açılışta dosyayı okur (yoksa dizin+dosya oluşturur, boş listeyle başlar).
Her ekleme/silmeden sonra tüm dosya yeniden yazılır (küçük veri seti için
basit ve yeterince güvenli — kısmi yazma/kilit karmaşıklığına gerek yok).
Bozuk/ayrıştırılamayan satırlar atlanır, program çökmez.

## Modüller

- `src/main.c` — ncurses init/teardown, ana olay döngüsü, mod (NORMAL /
  COMMAND / INPUT) yönetimi, tuş işleme
- `src/calendar.c/.h` — saf tarih matematiği: `days_in_month`,
  `first_weekday_of_month`, artık yıl kontrolü, ay adı tablosu. ncurses'a
  bağımlı değildir.
- `src/tasks.c/.h` — `Task` struct, dosyadan yükle/kaydet, tarihe göre
  görevleri sorgula, ekle/sil
- `src/ui.c/.h` — grid çizimi (kenarlıklar, gün hücreleri, seçim vurgusu),
  komut/durum satırı çizimi
- `Makefile` — `make` → `tcal` binary'si (`gcc ... -lncurses`)

## Hata yönetimi

- Dosya/dizin yoksa oluşturulur.
- Bozuk satırlar log'lanmadan atlanır (sessizce yok sayılır — küçük ölçekli
  bir araç için yeterli).
- Terminal çok küçükse (grid'in sığmayacağı kadar) ekranda kısa bir uyarı
  gösterilir, çökme yerine.

## Test

Ayrı bir otomatik test çatısı kurulmuyor (kapsam küçük). `calendar.c`
içindeki tarih fonksiyonları ncurses'tan bağımsız/saf olacak şekilde
yazılır, ileride istenirse kolayca test edilebilir. Doğrulama, uygulamayı
terminalde çalıştırıp manuel olarak yapılacak.
