" 行番号を表示
set number
" UTF-8でエンコーディング
set encoding=utf-8
" iTerm2用にシンタックスハイライトが出るように設定
set term=xterm-256color
" シンタクッスハイライト
syntax enable
syntax on
" titleを表示
set title
" ヤンクでクリップボードにコピー
set clipboard=unnamed,autoselect
" vimの短形選択で文字がなくても右へすすめる
set virtualedit=block
" wildmenuオプションを有効
set wildmenu
" 自動インデント
set autoindent
" インデント幅
set shiftwidth=2
" タブキーの挿入文字数を指定
set softtabstop=2
" カッコを補完する
inoremap { {}<LEFT>
inoremap ( ()<LEFT>
" "や'を補完する
inoremap " ""<LEFT>
inoremap ' ''<LEFT>
" 検索文字列をハイライト
set hlsearch