# vsjpeg.x

## これはなに？

X68000 + Human68k で動く JPSG ローダです。黒歴史として置いておきます。

v0.10 / v0.12 / v0.15 のソースをコミットしてあります。古いバージョンは手元になかったので、ネットで拾いました。ははは。

以下は、当時のアーカイブに同梱されていた vsjpeg.doc です (一部修正してあります) 。

```
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━

			       JPEGローダ
			  vsjpeg.x version 0.15
		      Copyright 1998, 2001 Igarashi

━━━━━━┯━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
これはなに？│
──────┘
　単なる JPEG ローダです。WebXpression で progressive JPEG を表示させた
かったので、作ってみました。

　でも、遅いです。ものすごく遅いです。

━━━━━━┯━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
インストール│
──────┘
　vsjpeg.x を、環境変数 path の指すディレクトリへコピーしてください。

━━━┯━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
使い方│
───┘
	JPEGローダ vsjpeg.x version 0.15 Copyright 1998, 2001 Igarashi
	usage:	vsjpeg [option] filename
		-L[x1,y1,[x2,y2]]	画面に展開・表示（デフォルト）
		-VS$address		指定のメモリに展開
		-A			スクエア表示モード
		-Inx/ny			展開時の縮尺を指定（1/1,1/2,1/4 or 1/8）

    -L[x1,y1,[x2,y2]]

	画像を展開し、画面に表示します（デフォルト）。座標指定（省略可）
	は 10 進数で指定してください。"-VS" オプションとは同時に使用でき
	ません。

	X680x0 のグラフィック画面に収まらない部分は表示できません。

    -VS$address

	画面に表示せずに、メモリに展開します。アドレスは 16 進数で指定し
	てください。アドレス $400000 に展開する場合は "-VS$400000" と指
	定します。"-A" オプションとは同時に使用できません。

    -A

	画面表示の際に、画面モードをいわゆる正方形モードに変更します。
	"-VS" オプションとは同時に使用できません。

    -Inx/ny

	縮尺を指定します。現バージョンでは "1/1" "1/2" "1/4" "1/8" のい
	ずれかが指定できます。

━━━━┯━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
配布規定│
────┘
　ご自由にどうぞ。

━━┯━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
謝辞│
──┘
　正方形モードの実装には、JPEGED.R のソースを参考にしました。

　vsjpeg.x の開発にあたり、以下のツール / ライブラリ / 参考
文献を使用しました。作者の方々に感謝いたします。

* ツール
	VTwentyOne.sys version 1.36c+14 patchlevel 7
	MicroEMACS 3.10 j1.43 (rel.5c6)
	GNU Make version 3.77 [Human68k Release 2]
	egcs 2.95.2 しゃどう (β3)
	HAS060 version 3.09+85+12[g2as]
	HLK evolution version 3.01+14[g2lk]
	X68k Hi-Speed Archiver v1.37
	oar - Object ARchiver ver1.0.4
	PD KSH v4.9(X6_27)
	exec bit driver version 0.35
	Human68k ITA TOOLBOX
	<ＭＵＳＨ> ～ Ｍu's Ｈackworks ～
	GNU sh-utils 1.16
	diff - GNU diffutils version 2.7 (X6_10)
	X68k Source Code Debugger v3.01+12
	GDB for X680x0 (Human68k) version 1.01
* ライブラリ
	The Independent JPEG Group's JPEG software
		release 6b of 27-Mar-1998
	LIBC 1.1.32A ぱっち ＤＯＮ版 その２
	libgcc version x.xx patchlevel 6
```

## 連絡先

https://github.com/68fpjc/vsjpeg
