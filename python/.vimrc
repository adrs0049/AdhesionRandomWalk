" Local vimrc configuration
"

autocmd VimEnter * NERDTree
autocmd VimEnter * wincmd p

let &path.="./include"

set tabstop=2
set softtabstop=2
set shiftwidth=2
set noexpandtab

set colorcolumn=80
highlight ColorColumn ctermbg=darkgray
