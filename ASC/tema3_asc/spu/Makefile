#   Target
PROGRAM_spu := my_spu
LIBRARY_embed := lib_my_spu.a


IMPORTS := -lmisc -lm
# include partea care face toata treaba pe baza definitiilor de mai sus

ifdef CELL_TOP
            include $(CELL_TOP)/buildutils/make.footer
else
            include ../../../../buildutils/make.footer
endif
