// @ts-check
/// <reference path="main.d.ts" />

class ContentBuilder {

    /**
     * @private
     * @type {string[]}
     */
    html;

    /**
     * @param {Schema} schema
     */
    constructor(schema) {
        this.html = [
            "<h1>Table of Contents</h1><hr>",
            this.createPropertyList("properties", schema.properties, "#/properties")
        ];

        if (schema.definitions) {
            this.html.push(this.createPropertyList("definitions", schema.definitions, "#/definitions"));
        }
    }

    /**
     * @private
     * @param {string} name
     * @param {Path} path
     * @returns {string}
     */
    createPropertyLink(name, path) {
        return `<b><a href="${path}">${name}</a></b>`;
    }

    /**
     * @private
     * @param {string} name
     * @param {Record<string, AnySchema> | AnySchema[]} properties
     * @param {Path} path
     * @param {string} prefix
     * @returns {string}
     */
    createPropertyList(name, properties, path, prefix = "") {
        return `<details class="list">
            <summary><b>${this.createPropertyLink(name, path)}</b></summary>
            ${Object.entries(properties).map(([key, schema]) => {
                if ("type" in schema) {
                    if (schema.type == "array") {
                        return this.createPropertyList(prefix + key, Array.isArray(schema.items) ? schema.items : {
                            item: schema.items
                        }, `${path}/${key}`);
                    } else if (schema.type == "object") {
                        return this.createPropertyList(prefix + key, schema.properties, `${path}/${key}`);
                    }
                } else if ("oneOf" in schema) {
                    return this.createPropertyList(prefix + key, schema.oneOf, `${path}/${key}`, "option ");
                }

                return this.createPropertyLink(prefix + key, `${path}/${key}`);
            }).join("\n")}
        </details>`;
    }

    /**
     * @public
     * @returns {string}
     */
    toHtml() {
        return this.html.join("\n");
    }
}

class SchemaBuilder {

    /**
     * @private
     * @type {string[]}
     */
    html;

    /**
     * @param {Schema} schema
     */
    constructor(schema) {
        this.html = [
            `<h1>${schema.title}</h1><hr>`,
            schema.description ?? "",
            `<h2 id="/properties">Properties</h2><hr>`,
            ...Object.entries(schema.properties).map(
                ([key, definition]) => this.anyBlock(`${key}`, definition, schema.required?.includes(key) ?? false, `#/properties/${key}`)
            )
        ];

        if (schema.definitions) {
            this.html.push(`<h2 id="/definitions">Definitions</h2><hr>`, ...Object.entries(schema.definitions).map(
                ([key, definition]) => this.anyBlock(`${key}`, definition, schema.required?.includes(key) ?? false, `#/definitions/${key}`)
            ));
        }
    }

    /**
     * @private
     * @param {string} type
     * @param {string} name
     * @param {string[]} rules
     * @param {string | undefined} description
     * @param {boolean} required
     * @param {Path} path
     * @returns {string}
     */
    createPropertyBlock(type, name, rules, description, required, path) {
        return `<div id=${path.slice(1)} class="property">
            <b class="name-info">
                <span class="type" style="background-color: var(--type-${type.split(":")[0].toLowerCase()})">${type.slice(0, 1).toUpperCase()}${type.slice(1)}</span>
                <span class="name">${name}</span>
                ${required ? ` <small><i>required</i></small>` : ""}
            </b>
            <div class="properties">
                ${rules.join("\n")}
            </div>
            ${description ? `<div class="description">${description}</div>` : ""}
        </div>`;
    }

    /**
     * @private
     * @param {string} name
     * @param {string | undefined} value
     * @param {string} type
     * @returns {string}
     */
    createRuleBlock(name, value, type = "any") {
        if (value) {
            return `<div class="rule"><p><b>${name}: </b><span ${type == "any" ? "" : `style="color: var(--value-${type})"`}>${value}</span></p></div>`;
        } else {
            return "";
        }
    }

    /**
     * @private
     * @param {string} summary
     * @param {string[]} properties
     * @returns {string}
     */
    createDefinitionDetails(summary, properties) {
        return `<details class="object"><summary>${summary}</summary><div>${properties.join("\n")}</div></details>`;
    }

    /**
     * @private
     * @param {string} name
     * @param {AnySchema} schema
     * @param {Path} path
     * @param {boolean} required
     * @returns {string}
     */
    anyBlock(name, schema, required, path) {
        const rules = Object.entries(schema).filter(([key]) => ![
            "properties",
            "required",
            "items",
            "oneOf",
            "type",
            "pattern",
            "description",
            "custom_default",
            "$ref"
        ].includes(key)).map(([key, value]) => this.createRuleBlock(
            key.substring(0, 1).toUpperCase() + key.substring(1).replace(/([A-Z])/g, " $1"),
            Array.isArray(value) ? value.map((entry) => `<span style="color: var(--value-${typeof entry})">${entry}</span>`).join(", ") : value.toString(),
            typeof value
        ));

        if ("custom_default" in schema && schema.custom_default != undefined) {
            const [ base, comment ] = schema.custom_default.toString().split("[");

            rules.push(this.createRuleBlock("Default", `<a href="${base}">${base}</a>` + (comment ? `<br><i>${comment.slice(0, -1)}</i>` : "")));
        }

        if ("pattern" in schema) {
            rules.push(this.createRuleBlock("Pattern", `<span class="pattern">/${schema.pattern}/</span>`));
        }

        if ("oneOf" in schema) {
            return this.createDefinitionDetails(
                this.createPropertyBlock("One-Of", name, rules, schema.description, required, path),
                Object.entries(schema.oneOf).map(([index, item]) => this.anyBlock(`<small><i>Option ${index}</i></small>`, item, required, `${path}/${index}`))
            );
        } else if ("$ref" in schema) {
            return this.createPropertyBlock(`Ref: <a class="ref" href="${schema.$ref}">#${schema.$ref.split("/").pop()}</a>`, name, rules, schema.description, required, path);
        } else if (schema.type == "number" || schema.type == "integer" || schema.type == "string" || schema.type == "boolean") {
            return this.createPropertyBlock(schema.type, name, rules, schema.description, required, path);
        } else if (schema.type == "array") {
            const block = this.createPropertyBlock(schema.type, name, rules, schema.description, required, path);

            if (Array.isArray(schema.items)) {
                return this.createDefinitionDetails(block, schema.items.map(
                    (item, index) => this.anyBlock(index.toString(), item, index < (schema.minItems ?? 0), `${path}/${index}`)
                ));
            } else {
                return this.createDefinitionDetails(block, [ this.anyBlock("item", schema.items, Boolean(schema.minItems), `${path}/item`) ]);
            }
        } else {
            return this.createDefinitionDetails(
                this.createPropertyBlock(schema.type, name, rules, schema.description, required, path),
                Object.entries(schema.properties).map(([key, property]) => this.anyBlock(key, property, schema.required?.includes(key) ?? false, `${path}/${key}`))
            );
        }
    }

    /**
     * @public
     * @returns {string}
     */
    toHtml() {
        return this.html.join("\n");
    }
}

(async () => {
    const sideElement = document.getElementById("side");
    const mdElement = document.getElementById("md");
    let schema = sessionStorage.getItem("schema");

    if (!sideElement || !mdElement) {
        throw new Error("Element not found.");
    }

    if (schema) {
        console.log("Schema loaded from cache.");
    } else {
        sessionStorage.setItem("schema", schema = JSON.stringify(await (await fetch("https://raw.githubusercontent.com/SMJSGaming/GDIntercept/main/theme.schema.json")).json()));
    }

    const schemaObj = JSON.parse(schema);

    sideElement.innerHTML = new ContentBuilder(schemaObj).toHtml();
    mdElement.innerHTML = new SchemaBuilder(schemaObj).toHtml();
    document.getElementById("menu-button")?.addEventListener("click", () => document.getElementById("side")?.classList.toggle("open"));
})();