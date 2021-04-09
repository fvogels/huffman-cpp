require 'asciidoctor'
require 'asciidoctor/extensions'


class TaskBlock < Asciidoctor::Extensions::BlockProcessor
  use_dsl
  named :TASK
  on_context :example

  def process(parent, reader, attrs)
    attrs = {**attrs, "name" => 'task', "caption" => 'test'}
    create_block(parent, :admonition, reader.readlines, attrs, content_model: :compound)
  end
end

class TaskBlockDocinfoProcessor < Asciidoctor::Extensions::DocinfoProcessor
  use_dsl

  def process doc
    <<~END
      <style>
        .admonitionblock.task td.icon .title::before {
          content: "🔨";
          color: #871452;
        }
      </style>'
    END
  end
end

Asciidoctor::Extensions.register do
  block TaskBlock
  docinfo_processor TaskBlockDocinfoProcessor if document.basebackend? 'html'
end
