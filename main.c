#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <ctype.h>

#define HEDEF_GUN 15

/*Durum degiskenleri*/
int saglik, enerji, oksijen, su, yedekParca, modulHasari;
int kilitMuhurlu;          /* 0 = acik, 1 = muhurlu */
int gun, tur;

/*Zorluk seviyesine bagli degerler*/
int firtinaTur;            
int firtinaHasari;         
int canEsik;              
int kesifBonus;            

/* oyun durumu: 0 devam, 1 kazandi, 2 kaybetti, 3 cikis */
int oyunDurumu = 0;



int satirTemizle()
{
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {
        /* giris tamponunu bosalt */
    }
    return 0;
}

int sinirla()
{
    if (saglik < 0)       saglik = 0;
    if (saglik > 100)     saglik = 100;
    if (enerji < 0)       enerji = 0;
    if (enerji > 100)     enerji = 100;
    if (oksijen < 0)      oksijen = 0;
    if (oksijen > 100)    oksijen = 100;
    if (modulHasari < 0)  modulHasari = 0;
    if (modulHasari > 100) modulHasari = 100;
    if (su < 0)           su = 0;
    if (yedekParca < 0)   yedekParca = 0;
    return 0;
}

int kisaDurum()
{
    printf("\n[Gun: %d | Tur: %d] Saglik:%d Enerji:%d Oksijen:%d Su:%d Parca:%d Hasar:%d Kilit:%s\n",
           gun, tur, saglik, enerji, oksijen, su, yedekParca, modulHasari,
           kilitMuhurlu == 1 ? "MUHURLU" : "ACIK");
    return 0;
}

/* ------------------------------------------------------------ */
/* Zorluk secimi (switch-case)                                   */
/* ------------------------------------------------------------ */
int zorlukSec()
{
    char secim;
    int gecerli;

    do {
        gecerli = 1;
        printf("Zorluk seviyesi secin (K: Kolay, N: Normal, Z: Zor): ");
        if (scanf(" %c", &secim) != 1) {
            exit(0);   /* giris sonu */
        }
        satirTemizle();

        switch (secim) {
            case 'k':
            case 'K':
                saglik = 100; enerji = 100; oksijen = 100;
                su = 20; yedekParca = 4; modulHasari = 0;
                firtinaTur = 3; firtinaHasari = 5; canEsik = 15; kesifBonus = 10;
                printf("Kolay seviye secildi.\n");
                break;
            case 'n':
            case 'N':
                saglik = 100; enerji = 90; oksijen = 100;
                su = 12; yedekParca = 3; modulHasari = 10;
                firtinaTur = 5; firtinaHasari = 8; canEsik = 20; kesifBonus = 0;
                printf("Normal seviye secildi.\n");
                break;
            case 'z':
            case 'Z':
                saglik = 100; enerji = 80; oksijen = 90;
                su = 6; yedekParca = 1; modulHasari = 20;
                firtinaTur = 7; firtinaHasari = 12; canEsik = 25; kesifBonus = -10;
                printf("Zor seviye secildi.\n");
                break;
            default:
                printf("UYARI: Gecersiz seviye! Lutfen K, N veya Z girin.\n");
                gecerli = 0;
                break;
        }
    } while (gecerli == 0);
    return 0;
}

/* ------------------------------------------------------------ */
/* G - Kesif                                                     */
/* Donus: 1 = tur harcandi, 0 = komut iptal                      */
/* ------------------------------------------------------------ */
int kesif()
{
    int zarYaralanma, zarKesif, suEsik, parcaEsik;

    if (enerji < 15) {
        printf("Yetersiz enerji! Kesif icin en az 15 enerji gerekir. Komut iptal.\n");
        return 0;
    }

    enerji  -= 15;
    oksijen -= 10;

    /* Su bulma esigi enerji ve oksijen durumuna gore belirlenir */
    if (enerji > 40 && oksijen > 30) {
        suEsik = 45 + kesifBonus;
    } else {
        suEsik = 20 + kesifBonus;
    }
    parcaEsik = suEsik + 20;

    /* Yaralanma kontrolu */
    zarYaralanma = rand() % 100;
    if (zarYaralanma < 10 || modulHasari > 70) {
        saglik -= 10;
        printf("Zar: %d / Esik: 10 (veya hasar>70) -> YARALANDIN! Saglik -10\n", zarYaralanma);
    } else {
        printf("Zar: %d / Esik: 10 (veya hasar>70) -> Yaralanma yok\n", zarYaralanma);
    }

    /* Kesif sonucu: 3 farkli sonuc */
    zarKesif = rand() % 100;
    if (zarKesif < suEsik) {
        su += 5;
        printf("Zar: %d / Esik: %d -> BASARILI: 5 birim su buldun!\n", zarKesif, suEsik);
    } else if (zarKesif < parcaEsik) {
        yedekParca += 1;
        printf("Zar: %d / Esik: %d -> BASARILI: 1 yedek parca buldun!\n", zarKesif, parcaEsik);
    } else {
        printf("Zar: %d / Esik: %d -> BASARISIZ: Eli bos dondun.\n", zarKesif, parcaEsik);
    }
    return 1;
}

/* ------------------------------------------------------------ */
/* O - Oksijen uretimi (2 su -> 3 oksijen)                       */
/* ------------------------------------------------------------ */
int oksijenUret()
{
    int miktar, kullanilan, artik, maxSu;

    if (enerji < 10) {
        printf("Yetersiz enerji! Oksijen uretimi icin 10 enerji gerekir. Komut iptal.\n");
        return 0;
    }
    if (su < 2) {
        printf("Yeterli su yok! En az 2 birim su gerekir. Komut iptal.\n");
        return 0;
    }

    printf("Mevcut su: %d. Kac birim su kullanilsin? ", su);
    if (scanf("%d", &miktar) != 1) {
        satirTemizle();
        printf("Gecersiz giris! Komut iptal.\n");
        return 0;
    }
    satirTemizle();

    if (miktar < 2) {
        printf("En az 2 birim su girmelisiniz. Komut iptal.\n");
        return 0;
    }
    if (miktar > su) {
        printf("Mevcut sudan fazla miktar girilemez! Komut iptal.\n");
        return 0;
    }

    /* Mod operatoru: tekse 1 birim artik olarak depoda kalir */
    artik = miktar % 2;
    kullanilan = miktar - artik;

    /* Oksijen 100'u asmasin: gereksiz su harcanmasin */
    maxSu = ((100 - oksijen) / 3) * 2;
    if (kullanilan > maxSu) {
        kullanilan = maxSu;
        printf("Oksijen deposu dolmak uzere, sadece %d birim su kullanilacak.\n", kullanilan);
    }

    if (kullanilan <= 0) {
        printf("Oksijen deposu zaten dolu. Komut iptal.\n");
        return 0;
    }

    su      -= kullanilan;
    oksijen += (kullanilan / 2) * 3;
    enerji  -= 10;

    printf("%d birim su kullanildi, %d birim oksijen uretildi.", kullanilan, (kullanilan / 2) * 3);
    if (artik == 1) {
        printf(" (1 birim su artik olarak depoda kaldi)");
    }
    printf("\n");
    return 1;
}

/* ------------------------------------------------------------ */
/* T - Tamir                                                     */
/* ------------------------------------------------------------ */
int tamir()
{
    int kullanilan, onarim;

    if (yedekParca <= 0) {
        printf("Yedek parca yok! Tamir yapilamaz. Komut iptal.\n");
        return 0;
    }
    if (enerji < 10) {
        printf("Yetersiz enerji! Tamir icin 10 enerji gerekir. Komut iptal.\n");
        return 0;
    }
    if (modulHasari == 0) {
        printf("Modulde hasar yok, tamire gerek yok. Komut iptal.\n");
        return 0;
    }

    /* Hasar kademeleri: IF - ELSE IF - ELSE */
    if (modulHasari <= 30) {
        /* Hafif hasar: 1 parca, parca basina 15 onarim */
        kullanilan = 1;
        onarim = kullanilan * 15;
        printf("Hafif hasar tamiri: ");
    } else if (modulHasari <= 70) {
        /* Orta hasar: en fazla 2 parca, parca basina 18 onarim */
        kullanilan = (yedekParca >= 2) ? 2 : 1;
        onarim = kullanilan * 18;
        printf("Orta hasar tamiri: ");
    } else {
        /* Agir hasar: en fazla 3 parca, parca basina 20 onarim */
        kullanilan = (yedekParca >= 3) ? 3 : yedekParca;
        onarim = kullanilan * 20;
        printf("Agir hasar tamiri: ");
    }

    yedekParca  -= kullanilan;
    modulHasari -= onarim;
    enerji      -= 10;
    printf("%d parca harcandi, %d hasar onarildi.\n", kullanilan, onarim);
    return 1;
}

/* ------------------------------------------------------------ */
/* D - Durum raporu (tur harcamaz)                               */
/* ------------------------------------------------------------ */
int durumRaporu()
{
    printf("\n===== DURUM RAPORU =====\n");
    printf("Gun          : %d / %d\n", gun, HEDEF_GUN);
    printf("Tur          : %d\n", tur);
    printf("Saglik       : %d\n", saglik);
    printf("Enerji       : %d\n", enerji);
    printf("Oksijen      : %d\n", oksijen);
    printf("Su           : %d birim\n", su);
    printf("Yedek parca  : %d\n", yedekParca);
    printf("Modul hasari : %d\n", modulHasari);
    printf("Hava kilidi  : %s\n", kilitMuhurlu == 1 ? "MUHURLU" : "ACIK");

    if (oksijen < 25)     printf("!!! KRITIK: Oksijen cok dusuk!\n");
    if (saglik < 30)      printf("!!! KRITIK: Saglik cok dusuk!\n");
    if (modulHasari > 70) printf("!!! KRITIK: Modul hasari cok yuksek!\n");
    if (enerji < 20)      printf("!!! KRITIK: Enerji cok dusuk!\n");

    if (oksijen >= 25 && saglik >= 30 && modulHasari <= 70 && enerji >= 20) {
        printf("TUM SISTEMLER NORMAL\n");
    }
    printf("========================\n");
    return 0;
}

/* ------------------------------------------------------------ */
/* U - Uyu / dinlen                                              */
/* ------------------------------------------------------------ */
int uyu()
{
    enerji += 25;

    if (oksijen >= 20) {
        saglik += 15;
        if (su < 3) {              /* ic ice IF: susuzluk etkisi */
            saglik -= 5;
            printf("Susuzluk dinlenmeni olumsuz etkiledi (Saglik -5).\n");
        }
        printf("Dinlendin: Enerji +25, Saglik +15.\n");
    } else {
        saglik -= 10;
        printf("Oksijen 20'nin altinda! Dinlenirken sagligin azaldi (Saglik -10), Enerji +25.\n");
    }

    oksijen -= 5;                  /* oksijen tuketimi devam eder */
    printf("Oksijen tuketimi: -5\n");
    return 1;
}

/* ------------------------------------------------------------ */
/* K - Kum firtinasi (FOR dongusu)                               */
/* ------------------------------------------------------------ */
int firtina()
{
    int i, zar, carpan, kayip;
    int e1, e2, e3;
    int hasarTur = 0;
    int saglikBas = saglik, oksijenBas = oksijen, hasarBas = modulHasari;

    printf("\n*** KUM FIRTINASI BASLADI (%d tur) ***\n", firtinaTur);

    for (i = 1; i <= firtinaTur; i++) {
        zar = rand() % 100;

        /* Cift numarali turlarda ruzgar siddetlenir, hasar 2 kat */
        if (i % 2 == 0) {
            carpan = 2;
        } else {
            carpan = 1;
        }

        /* Esikler oyuncunun durumuna gore degisir */
        e1 = canEsik;
        if (modulHasari > 70 || saglik < 30) {
            e1 += 10;              /* zayif durumda can kaybi ihtimali artar */
        }
        e2 = e1 + 20;              /* oksijen sizintisi */
        e3 = e2 + 20;              /* modul hasari */

        printf("Tur %d%s - Zar: %d / Esik: %d -> ", i,
               carpan == 2 ? " (SIDDETLI RUZGAR)" : "", zar, e3);

        if (zar < e1) {
            kayip = firtinaHasari * carpan;
            saglik -= kayip;
            hasarTur++;
            printf("CAN KAYBI: Saglik -%d\n", kayip);
        } else if (zar < e2) {
            if (kilitMuhurlu == 1) {
                printf("Hava kilidi muhurlu, oksijen sizintisi atlandi.\n");
                continue;          /* bu tur icin sizinti olayi atlanir */
            }
            kayip = firtinaHasari * carpan;
            oksijen -= kayip;
            hasarTur++;
            printf("OKSIJEN SIZINTISI: Oksijen -%d\n", kayip);
        } else if (zar < e3) {
            kayip = firtinaHasari * carpan;
            modulHasari += kayip;
            hasarTur++;
            printf("MODUL HASARI: Hasar +%d\n", kayip);
        } else {
            printf("KORUNDUN: Firtina zarar vermedi.\n");
        }

        sinirla();

        if (saglik == 0 || oksijen == 0) {
            printf("!!! Saglik veya oksijen tukendi, firtina donguden cikiliyor!\n");
            break;                 /* dongu derhal sonlanir */
        }
    }

    printf("--- Firtina Ozeti ---\n");
    printf("Hasar alinan tur sayisi : %d\n", hasarTur);
    printf("Kaybedilen saglik       : %d\n", saglikBas - saglik);
    printf("Kaybedilen oksijen      : %d\n", oksijenBas - oksijen);
    printf("Artan modul hasari      : %d\n", modulHasari - hasarBas);
    return 1;
}

/* ------------------------------------------------------------ */
/* H - Hava kilidi sifresi (DO-WHILE)                            */
/* ------------------------------------------------------------ */
int havaKilidi()
{
    char sifre, giris = 0;
    int deneme = 0;

    if (kilitMuhurlu == 1) {
        printf("Hava kilidi zaten muhurlu. Komut iptal.\n");
        return 0;
    }
    if (enerji < 15) {
        printf("Yetersiz enerji! Hava kilidi icin en az 15 enerji gerekir. Komut iptal.\n");
        return 0;
    }

    sifre = 'A' + (gun % 26);

    printf("\n--- HAVA KILIDI ---\n");
    printf("Ipucu: Sifre, alfabede A harfinden bugunun gun sayisi kadar ilerideki harftir.\n");
    printf("3 deneme hakkin var (tek karakter, A-Z).\n");

    do {
        deneme++;
        printf("Deneme %d/3 - Sifre: ", deneme);
        if (scanf(" %c", &giris) != 1) {
            giris = 0;
        }
        satirTemizle();
        giris = toupper(giris);    /* kucuk harf girisi de kabul edilir */

        if (giris != sifre) {
            enerji -= 5;
            if (enerji < 0) enerji = 0;
            if (giris < 'A' || giris > 'Z') {
                printf("Gecersiz karakter! Yanlis deneme, Enerji -5.\n");
            } else if (giris < sifre) {
                printf("Yanlis! Ipucu: ileri git. Enerji -5.\n");
            } else {
                printf("Yanlis! Ipucu: geri git. Enerji -5.\n");
            }
        }
    } while (giris != sifre && deneme < 3);

    if (giris == sifre) {
        kilitMuhurlu = 1;
        printf("SIFRE DOGRU! Hava kilidi muhurlendi.\n");
    } else {
        modulHasari += 20;
        printf("3 deneme de basarisiz! Modul hasari +20.\n");
    }
    return 1;
}

/* ------------------------------------------------------------ */
/* Sonuc raporu                                                  */
/* ------------------------------------------------------------ */
int sonucRaporu()
{
    int hayattaKalinanGun = gun - 1;   /* tamamlanan gun sayisi */
    int puan;

    if (hayattaKalinanGun < 0) hayattaKalinanGun = 0;
    puan = (hayattaKalinanGun * 100) + saglik + oksijen - modulHasari;

    printf("\n==============================\n");
    switch (oyunDurumu) {
        case 1:  printf("   KURTARMA EKIBI GELDI - KAZANDIN!\n"); break;
        case 2:  printf("   OYUN BITTI - KAYBETTIN\n");           break;
        default: printf("   OYUNDAN CIKILDI\n");                  break;
    }
    printf("==============================\n");
    printf("Hayatta kalinan gun : %d\n", hayattaKalinanGun);
    printf("Toplam tur          : %d\n", tur);
    printf("Saglik              : %d\n", saglik);
    printf("Enerji              : %d\n", enerji);
    printf("Oksijen             : %d\n", oksijen);
    printf("Su                  : %d\n", su);
    printf("Yedek parca         : %d\n", yedekParca);
    printf("Modul hasari        : %d\n", modulHasari);
    printf("PUAN                : %d\n", puan);
    printf("(Puan = gun*100 + saglik + oksijen - modulHasari)\n");
    return 0;
}

/* ------------------------------------------------------------ */
/* MAIN                                                          */
/* ------------------------------------------------------------ */
int main()
{
    char komut;
    int turHarcandi;

    srand((unsigned int)time(NULL));

    gun = 1;
    tur = 0;
    kilitMuhurlu = 0;

    printf("=====================================================\n");
    printf("   MARS KOLONISI YASAM DESTEK SIMULATORU\n");
    printf("   Kurtarma ekibi gelene kadar %d gun hayatta kal!\n", HEDEF_GUN);
    printf("=====================================================\n");

    zorlukSec();

    printf("\nKomutlar: G:Kesif O:Oksijen uret T:Tamir D:Durum U:Uyu K:Kum firtinasi H:Hava kilidi Q:Cikis\n");

    do {
        turHarcandi = 0;

        printf("\nKomut girin: ");
        if (scanf(" %c", &komut) != 1) {
            komut = 'Q';           /* giris sonu = cikis */
        }
        satirTemizle();

        switch (komut) {
            case 'g':
            case 'G':
                turHarcandi = kesif();
                break;
            case 'o':
            case 'O':
                turHarcandi = oksijenUret();
                break;
            case 't':
            case 'T':
                turHarcandi = tamir();
                break;
            case 'd':
            case 'D':
                durumRaporu();     /* tur harcamaz */
                break;
            case 'u':
            case 'U':
                turHarcandi = uyu();
                break;
            case 'k':
            case 'K':
                turHarcandi = firtina();
                break;
            case 'h':
            case 'H':
                turHarcandi = havaKilidi();
                break;
            case 'q':
            case 'Q':
                oyunDurumu = 3;
                break;
            default:
                printf("UYARI: Tanimsiz komut '%c'! Tur harcanmadi.\n", komut);
                break;
        }

        /* Gecerli komut calistiysa zaman akar */
        if (turHarcandi == 1) {
            tur++;

            /* Her 3 turda bir yeni gun */
            if (tur % 3 == 0) {
                gun++;
                oksijen -= 8;
                printf("\n>>> YENI GUN BASLADI: Gun %d (Oksijen -8)", gun);
                if (modulHasari > 70) {
                    saglik -= 5;
                    printf(" (Modul hasari >70: Saglik -5)");
                }
                printf("\n");
            }

            sinirla();
            kisaDurum();

            /* Oyun sonu kontrolu */
            if (saglik <= 0 || oksijen <= 0) {
                oyunDurumu = 2;
            } else if (gun > HEDEF_GUN) {
                oyunDurumu = 1;
            }
        }
    } while (oyunDurumu == 0);

    sonucRaporu();
    return 0;
}
