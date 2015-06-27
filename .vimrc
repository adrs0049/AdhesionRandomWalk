" Local vimrc configuration
"

autocmd VimEnter * NERDTree
autocmd VimEnter * wincmd p

let &path.="./include"

set tabstop=4
set softtabstop=4
set shiftwidth=4
set noexpandtab

set colorcolumn=80
highlight ColorColumn ctermbg=darkgray