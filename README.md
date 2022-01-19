# Voxel_Renderer

deci am incercat sa scot toata partea de textura si sa-l fac sa mearga pe pixeli pe un singur rand (fiindca inca e in 2d ca n-am adaugat nimic 3d), am modificat modul in care e salvat mapa(inainte era ca int pe cazuri random(1 era o textura, 2 alta, etc) acum e pe SDL_Color, daca map[i][j].r+map[i][j].g+map[i][j].b < 153(adica posibilitate de 25%, 153 fiind (255+255+255)/5) atunci voxelul e bloc full cu acea culoare, daca e mai mare ca 153 e spatiu gol.

am fct asta da inca am niste probleme cu framerateul si sincronizarea threadurilori crd
