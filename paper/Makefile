.PHONY: paper
paper:
	latexmk -pdf -outdir=out paper

.PHONY: pvc
pvc:
	latexmk -pdf -pvc -outdir=out -interaction=nonstopmode paper

.PHONY: clean
clean:
	rm -r out
