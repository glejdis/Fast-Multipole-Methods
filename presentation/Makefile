.PHONY: presentation
presentation:
	latexmk -pdf -outdir=out presentation

.PHONY: pvc
pvc:
	latexmk -pdf -pvc -outdir=out -interaction=nonstopmode presentation

.PHONY: clean
clean:
	rm -r out
